#include "agent_controller.h"

// Qt includes
#include <QDebug>
#include <QThread>
#include <QtMath>

// Local include
#include "synchronizer_base.h"
#include "collision_avoidance_manager.h"
#include "environment_manager.h"

using namespace AirPlug;
using namespace ClearPath;

namespace ClearPathApplication
{

class Q_DECL_HIDDEN AgentController::Private
{
public:

    Private()
        : board(nullptr),
          nbStep(0),
          synchronizer(nullptr),
          environmentMngr(nullptr)
    {
    }

    ~Private()
    {
        delete synchronizer;
        EnvironmentManager::clean();
    }

public:

    Board* board;

    int nbStep;

    SynchronizerBase* synchronizer;
    QHash<QString, CollisionAvoidanceManager*> localAgents;
    EnvironmentManager* environmentMngr;
};


/* -------------------------------------------------------------------------------------------------------------------------------------------------------------*/
AgentController::AgentController(QObject* parent)
    : ApplicationController(QLatin1String("RVO"), parent),
      d(new Private())
{
    setObjectName(QLatin1String("Agent Controller"));
}

AgentController::~AgentController()
{
    delete d;
}

void AgentController::init(const QCoreApplication& app)
{
    ApplicationController::init(app);

    // All local applications will subscribe to local NET
    m_communication->subscribeLocalHost(QLatin1String("NET"));

    ACLMessage pong(ACLMessage::PONG);
    QJsonObject content;
    content[QLatin1String("isNew")] = true;
    pong.setContent(content);

    sendMessage(pong, QString(), QString(), QString());

    // Create agents
    d->environmentMngr  = EnvironmentManager::init(0.25, 15, 10, 5, 2, 2, {0, 0});

    int sqrtNbAgent = int(sqrt(NB_AGENTS));

    for (int i = 0; i < sqrtNbAgent; i++)
    {
        for (int j = 0; j < sqrtNbAgent; ++j)
        {
            std::vector<double> startPoint = {double(m_optionParser.startPoint[0] + i * 10),  double(m_optionParser.startPoint[1] + j * 10)};

            QString agentName = siteID() + "_" + QString::number(i*sqrtNbAgent + j);

            CollisionAvoidanceManager* agent = d->environmentMngr->addAgent(agentName, startPoint, m_optionParser.goals.at(0));

            if (agent == nullptr)
            {
                qWarning() << "Cannot create" << agentName;
            }
            else
            {
                d->localAgents[agentName] = agent;
            }

        }
    }

    // Init synchronizer
    d->synchronizer     = new SynchronizerBase(siteID());

    connect(d->synchronizer, &SynchronizerBase::signalSendMessage,
            this,            &AgentController::slotSendMessage, Qt::DirectConnection);

    connect(d->synchronizer, &SynchronizerBase::signalSendState,
            this,            &AgentController::slotSendState, Qt::DirectConnection);

    connect(d->synchronizer, &SynchronizerBase::signalDoStep,
            this,            &AgentController::slotDoStep, Qt::DirectConnection);

    // wait for network is stable to init synchronizer
    QThread::msleep(5000);

    // desactivate temporary for watchdog testing
    //d->synchronizer->init();
}


void AgentController::slotReceiveMessage(Header& header, Message& message)
{
    ACLMessage* aclMessage = (static_cast<ACLMessage*>(&message));

    ACLMessage::Performative performative = aclMessage->getPerformative();

    switch (performative)
    {
        case ACLMessage::REQUEST_SNAPSHOT:
            sendLocalSnapshot();

            break;

        case ACLMessage::PING:
            aclMessage->setPerformative(ACLMessage::PONG);
            sendMessage(*aclMessage, QString(), QString(), QString());
            ++(*m_clock);

            break;

        default:
            VectorClock* senderClock = aclMessage->getTimeStamp();

            if (senderClock)
            {
                if (senderClock->getSiteID() == m_clock->getSiteID())
                {
                    // avoid round back
                    return;
                }

                m_clock->updateClock(*senderClock);
            }

            if (performative == ACLMessage::SYNC)
            {
                d->synchronizer->processSYNCMessage(*aclMessage);

                // decode the content of message to update KD Tree in environment manager
                QJsonObject content = aclMessage->getContent();

                if (senderClock && content.contains(QLatin1String("info")))
                {
                    QJsonArray infoArray = content[QLatin1String("info")].toArray();

                    for (int i = 0; i < infoArray.size(); ++i)
                    {
                        QString agent = infoArray[i].toObject().keys().at(0);
                        d->environmentMngr->setInfo(agent, infoArray[i].toObject()[agent].toObject());
                    }
                }
            }
            else if (performative == ACLMessage::SYNC_ACK)
            {
                d->synchronizer->processACKMessage(*aclMessage);
            }

            break;
    }
}

void AgentController::slotDoStep()
{
    updateGui();
    d->environmentMngr->update();

    for (QHash<QString, CollisionAvoidanceManager*>::const_iterator iter  = d->localAgents.cbegin();
                                                                    iter != d->localAgents.cend();
                                                                  ++iter)
    {
        iter.value()->update();
    }

    ++(*m_clock);
    if (! d->synchronizer->isInitiator())
    {
        // NOTE initiator don't send ack messages
        ACLMessage ack(ACLMessage::SYNC_ACK);
        ack.setTimeStamp(*m_clock);

        sendMessage(ack, QString(), QString(), QString());
    }
    else
    {
        // time step
        QThread::msleep(FRAME_PERIOD_MS);
    }

    ++d->nbStep;
    //qDebug() << siteID() << "do step";
}

void AgentController::slotSendMessage(ACLMessage& message)
{
    message.setTimeStamp(++(*m_clock));

    sendMessage(message, QString(), QString(), QString());
}

void AgentController::slotSendState(ACLMessage& message)
{
    // share local state to construct shared memory
    QJsonObject content = message.getContent();
    QJsonArray  infoArray;

    for (QHash<QString, CollisionAvoidanceManager*>::const_iterator iter  = d->localAgents.cbegin();
                                                                    iter != d->localAgents.cend();
                                                                  ++iter)
    {
        QJsonObject info;
        info[iter.key()] = iter.value()->getInfo();

        infoArray.append(info);
    }

    content[QLatin1String("info")] = infoArray;

    message.setContent(content);
    message.setTimeStamp(++(*m_clock));

    sendMessage(message, QString(), QString(), QString());

    //qDebug() << siteID() << "send info";
}

void AgentController::sendLocalSnapshot()
{
    qDebug() << siteID() << "record snapshot";
    QJsonObject localState = captureLocalState();

    ACLMessage stateMessage(ACLMessage::INFORM_STATE);

    stateMessage.setTimeStamp(*m_clock);
    stateMessage.setContent(localState);

    sendMessage(stateMessage, QString(), QString(), QString());
}

QJsonObject AgentController::captureLocalState() const
{
    ++(*m_clock);

    QJsonObject localState;
    localState[QLatin1String("options")] = m_optionParser.convertToJson();

    QJsonArray infoArray;

    for (QHash<QString, CollisionAvoidanceManager*>::const_iterator iter  = d->localAgents.cbegin();
                                                                    iter != d->localAgents.cend();
                                                                  ++iter)
    {
        QJsonObject info;
        info[iter.key()] = iter.value()->captureState();

        infoArray.append(info);
    }

    localState[QLatin1String("state")]   = infoArray;

    return localState;
}

void AgentController::setBoard(Board* board)
{
    d->board = board;
}

void AgentController::updateGui() const
{
    if (d->board)
    {
        QMap<QString, CollisionAvoidanceManager*> agents = d->environmentMngr->getAgents();

        for (QMap<QString, CollisionAvoidanceManager*>::const_iterator iter  = agents.cbegin();
                                                                       iter != agents.cend();
                                                                     ++iter)
        {
            d->board->updateAgentState(iter.key(), iter.value()->getPosition());
        }
    }
}

}
