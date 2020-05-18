#include "bas_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>


using namespace AirPlug;

namespace BasApplication
{

class Q_DECL_HIDDEN BasController::Private
{
public:

    Private()
        : timer(nullptr),
          nbSequence(0)
    {
        sharedMessage = QLatin1String("~");
        mutex         = new RicartLock();
    }

    ~Private()
    {
        delete timer;
        delete mutex;
    }

public:

    QTimer*               timer;

    // sharedMessage is shared variables, clients will cumulate  the nb of sequence, if the mutex is working, sum(nbSequences) = sharedMessage
    QString               sharedMessage;
    int                   nbSequence;

    RicartLock*           mutex;
};


/* -------------------------------------------------------------------------------------------------------------------------------------------------------------*/

BasController::BasController(QObject* parent)
    : ApplicationController(QLatin1String("BAS"), parent),
      d(new Private)
{
}

BasController::~BasController()
{
    delete d;
}

void BasController::init(const QCoreApplication& app)
{
    ApplicationController::init(app);

    if (m_optionParser.autoSend && m_optionParser.delay > 0)
    {
        slotActivateTimer(m_optionParser.delay);
    }

    connect(d->mutex, &RicartLock::signalResponse,
            this, &BasController::slotForwardMutex, Qt::DirectConnection);

    connect(d->mutex, &RicartLock::signalEnterRaceCondition,
            this, &BasController::slotEnterCriticalSection, Qt::DirectConnection);

    // All Bas will subscribe to local NET
    m_communication->subscribeLocalHost(QLatin1String("NET"));

    ACLMessage pong(ACLMessage::PONG);
    QJsonObject content;
    content[QLatin1String("isNew")] = true;
    pong.setContent(content);

    sendMessage(pong, QString(), QString(), QString());
}

void BasController::pause(bool b)
{
    m_optionParser.start = !b;

    if (b)
    {
        slotDeactivateTimer();
    }
}

void BasController::setMessage(const QString& msg)
{
    //d->sharedMessage = msg;

    //++(*m_clock);
}

void BasController::slotActivateTimer(int period)
{
    if (! d->timer)
    {
        d->timer = new QTimer(this);

        connect(d->timer, &QTimer::timeout,
                    this, &BasController::slotSendMessage);
    }

    m_optionParser.delay    = period;
    m_optionParser.autoSend = true;

    d->timer->start(period);

    ++(*m_clock);
}

void BasController::slotDeactivateTimer()
{
    m_optionParser.autoSend = false;
    m_optionParser.delay    = 0;

    if (d->timer)
    {
        d->timer->stop();
    }

    ++(*m_clock);
}

void BasController::slotPeriodChanged(int period)
{
    m_optionParser.delay = period;

    if (d->timer)
    {
        d->timer->setInterval(period);
    }

    ++(*m_clock);
}

void BasController::slotSendMessage()
{
    d->mutex->trylock((*m_clock));
}

void BasController::slotReceiveMessage(Header header, Message message)
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

            QJsonObject contents = aclMessage->getContent();

            d->sharedMessage = contents[QLatin1String("payload")].toString();
            //d->nbSequence    = contents[QLatin1String("nseq")].toInt();

            emit signalMessageReceived(header, message);
            //emit signalSequenceChange(d->nbSequence);

            break;
    }
}

QJsonObject BasController::captureLocalState() const
{
    ++(*m_clock);

    QJsonObject applicationState;

    applicationState[QLatin1String("sharedMessage")] = d->sharedMessage;
    applicationState[QLatin1String("nbSequence")]    = d->nbSequence;
    applicationState[QLatin1String("mutexQueue")]    = d->mutex->getPendingQueue();

    QJsonObject localState;
    localState[QLatin1String("options")]             = m_optionParser.convertToJson();
    localState[QLatin1String("state")]               = applicationState;

    return localState;
}

void BasController::sendLocalSnapshot()
{
    qDebug() << siteID() << "record snapshot";
    QJsonObject localState = captureLocalState();

    ACLMessage stateMessage(ACLMessage::INFORM_STATE);

    stateMessage.setTimeStamp(*m_clock);
    stateMessage.setContent(localState);

    sendMessage(stateMessage, QString(), QString(), QString());
}

void BasController::receiveMutexRequest(const ACLMessage& request) const
{
    VectorClock* senderClock = request.getTimeStamp();

    m_clock->updateClock(*senderClock);

    d->mutex->receiveExternalRequest(*senderClock);
}

void BasController::slotForwardMutex(const ACLMessage& message)
{
    // broadcast request to enter race condition
    sendMessage(message, QString(), QString(), QString());
}

void BasController::slotEnterCriticalSection()
{
    ++(*m_clock);

    ACLMessage message(ACLMessage::INFORM);

    // attache clock to the message
    message.setTimeStamp(*m_clock);

    QJsonObject contents;

    ++d->nbSequence;
    d->sharedMessage = QString::number(d->sharedMessage.toInt() + 1);

    contents[QLatin1String("payload")] =  d->sharedMessage;
    contents[QLatin1String("nseq")]    =  d->nbSequence;

    message.setContent(contents);

    // TODO: get what, where, who from user interface
    sendMessage(message, QString(), QString(), QString());

    emit signalMessageReceived(Header("NET", "BAS", Header::localHost), message);
    emit signalSequenceChange(d->nbSequence);

    d->mutex->unlock();
}


}
