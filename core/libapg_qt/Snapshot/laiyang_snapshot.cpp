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

    int msgCounter;

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

bool LaiYangSnapshot::processMessage(ACLMessage* message, bool isLocal)
{
    switch (message->getPerformative())
    {
        case ACLMessage::INFORM_STATE :

            return processStateMessage(message, isLocal);

        case ACLMessage::PREPOST_MESSAGE:

            break;

        default:
            // Normal message
            if (isLocal)
            {
                // receive message from local application ==> add color and send
                colorMessage(message);
            }
            else
            {
                // Receive from another NET ==> get color
                QString color = getColor(message);

                if (color == QLatin1String("red") && !d->recorded)
                {
                    requestSnapshot();
                }
                else if (color == QLatin1String("white") && !d->recorded)
                {
                    // prepost message

                }
            }

            // forward the message
            return true;
    }
}


/* ----------------------------------------------- Helper functions -----------------------------------------------*/

void LaiYangSnapshot::requestSnapshot()
{
    d->recorded = true;

    ACLMessage* command = new ACLMessage(ACLMessage::REQUEST_SNAPSHOT);

    emit signalRequestSnapshot(command);
}

void LaiYangSnapshot::colorMessage(ACLMessage* message)
{
    QJsonObject content = message->getContent();

    // append color field to the content of the message
    if (d->recorded)
    {
        content[QLatin1String("color")] = QLatin1String("red");
    }
    else
    {
        content[QLatin1String("color")] = QLatin1String("white");
    }

    message->setContent(content);
}

QString LaiYangSnapshot::getColor(ACLMessage* message) const
{
    QJsonObject content = message->getContent();

    QString color = content[QLatin1String("color")].toString();

    content.remove(QLatin1String("color"));

    message->setContent(content);

    return color;
}

bool LaiYangSnapshot::processStateMessage(ACLMessage* message, bool isLocal)
{
    VectorClock* timestamp = message->getTimeStamp();

    if (!timestamp)
    {
        qWarning() << "INFORM_STATE message doesn't contain timestamp.";

        return false;
    }

    QJsonObject state = timestamp->convertToJson();

    QJsonObject content = message->getContent();

    state[QLatin1String("state")] = content;

    if (d->initiator)
    {
        // Collect state
        collectState(state);

        return false;
    }
    else if (isLocal)
    {
        // Record State from local application
        collectState(state);
    }

    // Forward to initiator
    return true;
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
    //TODO
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

}
