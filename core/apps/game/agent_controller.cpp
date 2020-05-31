#include "agent_controller.h"

// Qt includes
#include <QTimer>
#include <QDebug>
#include <QThread>


using namespace AirPlug;

namespace GameApplication
{

class Q_DECL_HIDDEN AgentController::Private
{
public:

    Private(Board* board)
        : //timer(nullptr),
          board(board),
          nbSequence(0)
    {
        mutex         = new RicartLock();
    }

    ~Private()
    {
        //delete timer;
        delete mutex;
    }

public:

    //QTimer*     timer;

    // all agents shared the same Map
    Board*      board;
    Agent*      localAgent;

    int         nbSequence;

    RicartLock* mutex;
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

    if (m_optionParser.autoSend && m_optionParser.delay > 0)
    {
        // TODO handle auto play
    }

    connect(d->mutex, &RicartLock::signalResponse,
            this,     &AgentController::slotForwardMutex, Qt::DirectConnection);

    connect(d->mutex, &RicartLock::signalEnterRaceCondition,
            this,     &AgentController::slotEnterCriticalSection, Qt::DirectConnection);

    // All Bas will subscribe to local NET
    m_communication->subscribeLocalHost(QLatin1String("NET"));

    ACLMessage pong(ACLMessage::PONG);
    QJsonObject content;
    content[QLatin1String("isNew")] = true;
    pong.setContent(content);

    sendMessage(pong, QString(), QString(), QString());

    // init local agent
    d->localAgent = new Agent(siteID(), AGENT_RADIUS);
    d->localAgent->init();

    d->board->addAgent(siteID(), d->localAgent);

    connect(d->localAgent, &Agent::signalStateChanged,
            this,          &AgentController::slotSendMessage, Qt::DirectConnection);

    // Broadcast agent initial state to others
    slotSendMessage();
}


/*
void AgentController::pause(bool b)
{
    m_optionParser.start = !b;

    if (b)
    {
        slotDeactivateTimer();
    }
}
*/

/*

void AgentController::slotActivateTimer(int period)
{
    if (! d->timer)
    {
        d->timer = new QTimer(this);

        connect(d->timer, &QTimer::timeout,
                    this, &AgentController::slotSendMessage);
    }

    m_optionParser.delay    = period;
    m_optionParser.autoSend = true;

    d->timer->start(period);

    ++(*m_clock);
}



void AgentController::slotDeactivateTimer()
{
    m_optionParser.autoSend = false;
    m_optionParser.delay    = 0;

    if (d->timer)
    {
        d->timer->stop();
    }

    ++(*m_clock);
}

void AgentController::slotPeriodChanged(int period)
{
    m_optionParser.delay = period;

    if (d->timer)
    {
        d->timer->setInterval(period);
    }

    ++(*m_clock);
}

*/
void AgentController::slotSendMessage()
{
    d->mutex->trylock((*m_clock));
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

        case ACLMessage::REQUEST_MUTEX:
            if (aclMessage->getTimeStamp()->getSiteID() != siteID())
            {
                receiveMutexRequest(*aclMessage);
            }

            break;

        case ACLMessage::ACCEPT_MUTEX:
            //qDebug() << "receive accept mutex" << aclMessage->getContent();

            if (aclMessage->getContent()[QLatin1String("apps")].toArray().contains(m_clock->getSiteID()))
            {
                d->mutex->lock();
            }

            break;

        case ACLMessage::REFUSE_MUTEX:
            if (aclMessage->getContent()[QLatin1String("apps")].toArray().contains(m_clock->getSiteID()))
            {
                d->mutex->restart();
            }

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

            // TODO: receive Agent message
            QJsonObject contents = aclMessage->getContent();
            qDebug() << siteID() << "receive state from" << aclMessage->getSender();

            d->board->updateAgentState(State(contents));

            break;
    }
}

QJsonObject AgentController::captureLocalState() const
{
    ++(*m_clock);

    QJsonObject applicationState;

    // TODO capture game state

    applicationState[QLatin1String("mutexQueue")]    = d->mutex->getPendingQueue();

    QJsonObject localState;
    localState[QLatin1String("options")]             = m_optionParser.convertToJson();
    localState[QLatin1String("state")]               = applicationState;

    return localState;
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

void AgentController::receiveMutexRequest(const ACLMessage& request) const
{
    VectorClock* senderClock = request.getTimeStamp();

    m_clock->updateClock(*senderClock);

    d->mutex->receiveExternalRequest(*senderClock);
}

void AgentController::slotForwardMutex(const ACLMessage& message)
{
    // broadcast request to enter race condition
    sendMessage(message, QString(), QString(), QString());
}

void AgentController::slotEnterCriticalSection()
{
    ++(*m_clock);

    ACLMessage message(ACLMessage::INFORM);

    // attache clock to the message
    message.setTimeStamp(*m_clock);

    // TODO update all changes making since the last update to the network
    d->localAgent->move();
    QJsonObject contents = d->localAgent->getState().toJson();

    message.setContent(contents);
    qDebug() << siteID() << "send state";

    // TODO: get what, where, who from user interface
    sendMessage(message, QString(), QString(), QString());

    d->mutex->unlock();
}


}
