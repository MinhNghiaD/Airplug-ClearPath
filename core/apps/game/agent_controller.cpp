#include "agent_controller.h"

// Qt includes
#include <QTimer>
#include <QDebug>
#include <QThread>

// Local include
#include "synchronizer_base.h"

using namespace AirPlug;

namespace ClearPathApplication
{

class Q_DECL_HIDDEN AgentController::Private
{
public:

    Private(Board* board)
        : //timer(nullptr),
          board(board),
          nbSequence(0),
          synchronizer(nullptr)
    {
    }

    ~Private()
    {
        delete synchronizer;
    }

public:

    //QTimer*     timer;

    // all agents shared the same Map
    Board*      board;
    Agent*      localAgent;

    int         nbSequence;

    SynchronizerBase* synchronizer;
};


/* -------------------------------------------------------------------------------------------------------------------------------------------------------------*/
AgentController::AgentController(Board* board, QObject* parent)
    : ApplicationController(QLatin1String("BAS"), parent),
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
/*
    if (m_optionParser.autoSend && m_optionParser.delay > 0)
    {
        // TODO handle auto play
    }
*/
    d->synchronizer = new SynchronizerBase(siteID());
    // TODO Application 1 : setup synchronizer


    // All Bas will subscribe to local NET
    m_communication->subscribeLocalHost(QLatin1String("NET"));

    ACLMessage pong(ACLMessage::PONG);
    QJsonObject content;
    content[QLatin1String("isNew")] = true;
    pong.setContent(content);

    sendMessage(pong, QString(), QString(), QString());



    // wait for network is establish and init synchronizer
    QThread::msleep(5);
    d->synchronizer->init();

/*
    // init local agent
    d->localAgent = new Agent(siteID(), AGENT_RADIUS);
    d->localAgent->init();

    d->board->addAgent(siteID(), d->localAgent);

    connect(d->localAgent, &Agent::signalStateChanged,
            this,          &AgentController::slotSendMessage, Qt::DirectConnection);

    // Broadcast agent initial state to others
    slotSendMessage();
*/
}

void AgentController::slotReceiveMessage(Header header, Message message)
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
                // TODO Application 2 : decode the content of message to update KD Tree in environment manager
                // This message should have a field site ID, maxSpeed, position, velocity from sender's CollisionAvoidanceManager
                // The field is the same as in TODO Application 4

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
    // TODO Application 3: use CollisionAvoidanceManager to update position and move to the new position on the
}

void AgentController::slotSendMessage(ACLMessage& message)
{
    // TODO Application 4: collect maxSpeed, position, velocity from local CollisionAvoidanceManager
    // Put in QJsonObject and put it in the message (envelop) to send to NET
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
    // TODO Application 5: capture agent state


    QJsonObject localState;
    localState[QLatin1String("options")] = m_optionParser.convertToJson();
    localState[QLatin1String("state")]   = applicationState;

    return localState;
}
}
