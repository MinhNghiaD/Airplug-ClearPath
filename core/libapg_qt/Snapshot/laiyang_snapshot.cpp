#include "laiyang_snapshot.h"

#include <QDebug>
#include <QUuid>


#include "vector_clock.h"

namespace AirPlug
{

class Q_DECL_HIDDEN LaiYangSnapshot::Private
{
public:

    Private()
        : recorded(false),
          initiator(false),
          msgCounter(0)
    {
    }

    ~Private()
    {
    }

public:

    bool recorded;
    bool initiator;

    int  msgCounter;

    // System state will be encoded in Json object
    QHash<QUuid, QJsonObject> states;
    QHash<QUuid, QJsonObject> prepostMessages;
};

LaiYangSnapshot::LaiYangSnapshot()
    : QObject(nullptr),
      d(new Private())
{
}

LaiYangSnapshot::~LaiYangSnapshot()
{
    delete d;
}

void LaiYangSnapshot::init()
{
    d->initiator = true;

    requestSnapshot();
}

// TODO: consider wave to build broadcast system after forward to BAS
LaiYangSnapshot::ForwardPort LaiYangSnapshot::processMessage(ACLMessage* message, bool fromLocal)
{
    switch (message->getPerformative())
    {
        case ACLMessage::INFORM_STATE :

            //return processStateMessage(message, fromLocal);

        case ACLMessage::PREPOST_MESSAGE:

            processPrePostMessage(message);

            return ForwardPort::DROP;

        default:
            // Normal message
            if (fromLocal)
            {
                // receive message from local application ==> add color and send
                //colorMessage(message);

                return ForwardPort::NET;
            }
            else
            {
                /*
                // Receive from another NET ==> get color
                QString color = getColor(message);

                if (color == QLatin1String("red") && !d->recorded)
                {
                    requestSnapshot();
                }
                else if (color == QLatin1String("white") && !d->recorded)
                {
                    // prepost message
                    processPrePostMessage(message);
                }
                */

                return ForwardPort::BAS;
            }
    }
}

void LaiYangSnapshot::colorMessage(QJsonObject& messageContent)
{
    // append color field to the content of the message
    messageContent[QLatin1String("snapshotted")] = d->recorded;
}

void LaiYangSnapshot::getColor(QJsonObject& messageContent)
{
    bool snapshotted = messageContent[QLatin1String("snapshotted")].toBool();

    messageContent.remove(QLatin1String("snapshotted"));

    if (snapshotted && !d->recorded)
    {
        requestSnapshot();
    }
    else if (!snapshotted && d->recorded)
    {
        // prepost message TODO
        //processPrePostMessage(&message);
    }
}

bool LaiYangSnapshot::processStateMessage(const ACLMessage& message, bool fromLocal)
{
    VectorClock* timestamp = message.getTimeStamp();

    if (!timestamp)
    {
        qWarning() << "INFORM_STATE message doesn't contain timestamp.";

        return false;
    }

    QJsonObject state = timestamp->convertToJson();

    QJsonObject content = message.getContent();

    state[QLatin1String("state")] = content;

    if (d->initiator)
    {
        collectState(state);

        return false;
    }
    else if (fromLocal)
    {
        // Record State from local application
        collectState(state);
    }

    // Forward to initiator
    return true;
}


/* ----------------------------------------------- Helper functions -----------------------------------------------*/

void LaiYangSnapshot::requestSnapshot()
{
    d->recorded = true;

    // Chandy-Lamport marker
    ACLMessage* marker = new ACLMessage(ACLMessage::REQUEST_SNAPSHOT);

    emit signalRequestSnapshot(marker);
}





void LaiYangSnapshot::processPrePostMessage(const ACLMessage* message)
{
    VectorClock* timestamp = message->getTimeStamp();

    if (!timestamp)
    {
        qWarning() << "INFORM_STATE message doesn't contain timestamp.";

        return;
    }

    if (d->initiator)
    {
        QJsonObject prepost = timestamp->convertToJson();

        QJsonObject content = message->getContent();

        prepost[QLatin1String("payload")] = content;

        collectPrePostMessage(prepost);
    }
    else
    {
        if (message->getPerformative() == ACLMessage::PREPOST_MESSAGE)
        {
            emit signalForwardPrePost(message);
        }
        else
        {
            // mark as prepost and forward message to the network
            ACLMessage* prepostMessage = new ACLMessage(*message);

            prepostMessage->setPerformative(ACLMessage::PREPOST_MESSAGE);

            emit signalForwardPrePost(prepostMessage);
        }
    }
}

void LaiYangSnapshot::collectState(const QJsonObject& state)
{
    if (validateState(state))
    {
        QUuid uuid = QUuid(state[QLatin1String("siteID")].toString());

        d->states[uuid] = state;
    }
    else
    {
        qWarning() << "Inconherent Snapshot detected ---> drop state.";
    }
}

bool LaiYangSnapshot::validateState(const QJsonObject& state)
{
    QJsonObject timestamp = state;
    timestamp.remove(QLatin1String("state"));

    QString siteID = timestamp[QLatin1String("siteID")].toString();

    VectorClock newClock(timestamp);

    // verify conference of snapshot by coherence property of a cut
    for (QHash<QUuid, QJsonObject>::const_iterator iter  = d->states.cbegin();
                                                   iter != d->states.cend();
                                                   ++iter)
    {
        QJsonObject currentTimestamp = iter.value();
        currentTimestamp.remove(QLatin1String("state"));

        QString currentSiteID = currentTimestamp[QLatin1String("siteID")].toString();

        VectorClock currentClock(currentTimestamp);

        // every clock must be the most recent clock of its site
        if ( (newClock.getValue(siteID)        < currentClock.getValue(siteID))         ||
             (newClock.getValue(currentSiteID) > currentClock.getValue(currentSiteID)) )
        {
            return false;
        }
    }

    return true;
}

void LaiYangSnapshot::collectPrePostMessage(const QJsonObject& prepostMessage)
{
    // TODO : validate prepost if necessary
    QUuid uuid = QUuid(prepostMessage[QLatin1String("siteID")].toString());

    if (!d->prepostMessages.contains(uuid))
    {
        d->prepostMessages[uuid] = prepostMessage;
    }
}

}
