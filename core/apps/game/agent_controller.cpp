#include "agent_controller.h"

// Qt includes
//#include <QTimer>
#include <QDebug>
#include <QThread>

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

    Private(Board* board)
        : board(board),
          nbStep(0),
          synchronizer(nullptr),
          localAgent(nullptr),
          environmentMngr(nullptr)
    {
    }

    ~Private()
    {
        delete synchronizer;
        EnvironmentManager::clean();
    }

public:

    Board*      board;
    Agent*      guiAgent;

    int         nbStep;

    SynchronizerBase* synchronizer;
    CollisionAvoidanceManager* localAgent;
    EnvironmentManager* environmentMngr;
};


/* -------------------------------------------------------------------------------------------------------------------------------------------------------------*/
AgentController::AgentController(Board* board, QObject* parent)
    : ApplicationController(QLatin1String("RVO"), parent),
      d(new Private(board))
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

    // All Bas will subscribe to local NET
    m_communication->subscribeLocalHost(QLatin1String("NET"));

    ACLMessage pong(ACLMessage::PONG);
    QJsonObject content;
    content[QLatin1String("isNew")] = true;
    pong.setContent(content);

    sendMessage(pong, QString(), QString(), QString());

    // TODO init GUI
    //State agentState = d->guiAgent->getState();
    double agentPosX = 0.0;
    double agentPosY = 0.0;

    d->environmentMngr  = EnvironmentManager::init(0.25, 15, 10, 5, 2, 2, {0, 0});
    d->localAgent       = d->environmentMngr->addAgent(siteID(), m_optionParser.startPoint, m_optionParser.goals.at(0));

    // wait for network is establish and init synchronizer
    QThread::msleep(5000);
    d->synchronizer = new SynchronizerBase(siteID());

    connect(d->synchronizer, &SynchronizerBase::signalSendMessage,
            this,            &AgentController::slotSendMessage, Qt::DirectConnection);

    connect(d->synchronizer, &SynchronizerBase::signalSendState,
            this,            &AgentController::slotSendState, Qt::DirectConnection);

    connect(d->synchronizer, &SynchronizerBase::signalDoStep,
            this,            &AgentController::slotDoStep, Qt::DirectConnection);

    d->synchronizer->init();
}


void AgentController::slotReceiveMessage(Header& header, Message& message)
{
    ACLMessage* aclMessage = (static_cast<ACLMessage*>(&message));

    switch (aclMessage->getPerformative())
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

            // read sender's clock
            if (senderClock)
            {
                if (senderClock->getSiteID() == m_clock->getSiteID())
                {
                    // avoid round back
                    return;
                }

                m_clock->updateClock(*senderClock);
            }

            if (aclMessage->getPerformative() == ACLMessage::SYNC)
            {
                d->synchronizer->processSYNCMessage(*aclMessage);

                // decode the content of message to update KD Tree in environment manager
                QJsonObject content = aclMessage->getContent();

                if (senderClock && content.contains(QLatin1String("info")))
                {
                    //qDebug() << siteID() << "reveice infor from" << senderClock->getSiteID();
                    d->environmentMngr->setInfo(senderClock->getSiteID(), content[QLatin1String("info")].toObject());
                }
            }
            else if (aclMessage->getPerformative() == ACLMessage::SYNC_ACK)
            {
                d->synchronizer->processACKMessage(*aclMessage);
            }

            break;
    }
}

void AgentController::slotDoStep()
{
    ++(*m_clock);
/*
    QMap<QString, CollisionAvoidanceManager*> agents = d->environmentMngr->getAgents();

    for (QMap<QString, CollisionAvoidanceManager*>::const_iterator iter  = agents.cbegin();
                                                                   iter != agents.cend();
                                                                 ++iter)
    {
        qDebug() << siteID() << ": " << iter.key() << "position:" << iter.value()->getPosition();
    }
*/
    d->environmentMngr->update();
    d->localAgent->update();

    qDebug() << siteID() << "do Step";

    // TODO update position in GUI
    if (! d->synchronizer->isInitiator())
    {
        // NOTE initiator don't send ack messages
        ACLMessage ack(ACLMessage::SYNC_ACK);
        ack.setTimeStamp(*m_clock);

        sendMessage(ack, QString(), QString(), QString());
    }
    else
    {
        QThread::msleep(5000);
    }
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
    content[QLatin1String("info")] = d->localAgent->getInfo();

    message.setContent(content);
    message.setTimeStamp(++(*m_clock));

    sendMessage(message, QString(), QString(), QString());
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

    // capture agent state
    QJsonObject applicationState = d->localAgent->captureState();

    QJsonObject localState;
    localState[QLatin1String("options")] = m_optionParser.convertToJson();
    localState[QLatin1String("state")]   = applicationState;

    return localState;
}

}
