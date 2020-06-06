#include "agent_controller.h"

// Qt includes
#include <QTimer>
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
          nbSequence(0),
          synchronizer(nullptr),
          localAgent(nullptr),
          environmentMngr(nullptr)
    {
    }

    ~Private()
    {
        delete synchronizer;
        delete localAgent;
        delete environmentMngr;
    }

public:

    Board*      board;
    Agent*      guiAgent;

    int         nbSequence;

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

    d->synchronizer = new SynchronizerBase(siteID());

    connect(d->synchronizer, &SynchronizerBase::signalSendMessage,
            this,            &AgentController::slotSendMessage, Qt::DirectConnection);

    connect(d->synchronizer, &SynchronizerBase::signalSendState,
            this,            &AgentController::slotSendState, Qt::DirectConnection);

    connect(d->synchronizer, &SynchronizerBase::signalDoStep,
            this,            &AgentController::slotDoStep, Qt::DirectConnection);

    // TODO Application 2:  setup CollisionAvoidanceManager and EnvironmentManager

    // Agent State or std::vector<double>{0., 0.}
    // NOTE: GUI later
    //State agentState = d->guiAgent->getState();
    double agentPosX = 0.0;
    double agentPosY = 0.0;

    d->localAgent = new CollisionAvoidanceManager(
                        std::vector<double>{agentPosX, agentPosY},
                        std::vector<double>{0., 0.},
                        std::vector<double>{0., 0.},
                        0.,
                        0.,
                        0.,
                        0.,
                        0,
                        nullptr);

    // EnvironmentManager Constructor isn't ready yet
    // d->environmentMngr = new EnvironmentManager();


    // All Bas will subscribe to local NET
    m_communication->subscribeLocalHost(QLatin1String("NET"));

    ACLMessage pong(ACLMessage::PONG);
    QJsonObject content;
    content[QLatin1String("isNew")] = true;
    pong.setContent(content);

    sendMessage(pong, QString(), QString(), QString());



    // wait for network is establish and init synchronizer
    QThread::msleep(5000);
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

                // TODO Application 3: decode the content of message to update KD Tree in environment manager
                // decode the content of message to update KD Tree in environment manager
                // NOTE: by using CollisionAvoidanceManager::getInfo

                QJsonObject content = aclMessage->getContent();
                /*
                Content might be extracted from aclMessage

                info[QLatin1String("position")] = encodeVector(position);
                info[QLatin1String("velocity")] = encodeVector(velocity);
                info[QLatin1String("maxSpeed")] = maxSpeed;
                */
                QJsonObject info = d->localAgent->getInfo();

                d->environmentMngr->setInfo("TODO", info);

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
    // TODO Application 4: use CollisionAvoidanceManager to update position and move to the new position
    // Send SYNC_ACK Message back to initiator


    QThread::msleep(5000);

    qDebug() << siteID() << "do step";

    if (! d->synchronizer->isInitiator())
    {
        // NOTE initiator don't send ack messages
        ACLMessage ack(ACLMessage::SYNC_ACK);
        ack.setTimeStamp(*m_clock);

        sendMessage(ack, QString(), QString(), QString());
    }
}

void AgentController::slotSendMessage(ACLMessage& message)
{
    ++(*m_clock);

    sendMessage(message, QString(), QString(), QString());
}

void AgentController::slotSendState(ACLMessage& message)
{
    // TODO Application 4: collect maxSpeed, position, velocity from local CollisionAvoidanceManager
    // Put in QJsonObject and put it in the message (envelop) to send to NET
    QJsonObject info = d->localAgent->getInfo();
    // NOTE message performative already prepared in message


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

    QJsonObject applicationState;
    // TODO Application 6: capture agent state
    // NOTE: by using CollisionAvoidanceManager::captureState


    QJsonObject localState;
    localState[QLatin1String("options")] = m_optionParser.convertToJson();
    localState[QLatin1String("state")]   = applicationState;

    return localState;
}
}
