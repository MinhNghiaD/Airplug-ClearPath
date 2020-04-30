#include "laiyang_snapshot.h"

// Qt includes
#include <QDebug>

// libapg include
#include "vector_clock.h"

namespace AirPlug
{

class Q_DECL_HIDDEN LaiYangSnapshot::Private
{
public:

    Private()
        : recorded(false),
          initiator(false),
          msgCounter(0),
          nbWaitPrepost(0),
          nbApp(0),
          nbNeighbor(0)
    {
    }

    ~Private()
    {
    }

public:


    /**
     * @brief validateState :  verify if a state is valide to record
     * @param state
     * @return
     */
    bool validateState(const QJsonObject& state) const;

    /**
     * @brief collectState: collect a local state
     * @param state
     *
     * A State object should have the form of :
     * {
     *     siteID : QString
     *     clock  : vector clock
     *     state  : {
     *                  options       : application option
     *                  local varable : jsonObject
     *              }
     * }
     *
     */
    bool collectState(const QJsonObject& state);

    /**
     * @brief verifyPrepost : reverify if a message is a prepost message becore recording snapshot
     * @param content
     * @return
     */
    bool verifyPrepost(const QJsonObject& content, QString& sender) const;

    /**
     * @brief allStateColltected : check if all state are collected
     * @return
     */
    bool allStateColltected() const;

    /**
     * @brief allPrepostCollected : verify if all prepost messages are collected
     * @return
     */
    bool allPrepostCollected() const;

    /**
     * @brief nbCollectedPrepost : return number of collected prepost message
     * @return
     */
    int nbCollectedPrepost() const;

public:

    bool recorded;
    bool initiator;

    int  msgCounter;
    int  nbWaitPrepost;

    int  nbApp;
    int  nbNeighbor;

    // System state will be encoded in Json object
    QHash<QString, QJsonObject> states;

    // Map of sender and its prepost messages
    QHash<QString, QVector<QJsonObject> > prepostMessages;
};



bool LaiYangSnapshot::Private::validateState(const QJsonObject& state) const
{
    QJsonObject timestamp = state;
    timestamp.remove(QLatin1String("state"));

    QString siteID = timestamp[QLatin1String("siteID")].toString();

    VectorClock newClock(timestamp);

    // verify conference of snapshot by coherence property of a cut
    for (QHash<QString, QJsonObject>::const_iterator iter  = states.cbegin();
                                                     iter != states.cend();
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

bool LaiYangSnapshot::Private::collectState(const QJsonObject& state)
{
    if (validateState(state))
    {
        QString siteID = state[QLatin1String("siteID")].toString();

        states[siteID] = state;

        return true;
    }
    else
    {
        qWarning() << "Inconherent Snapshot detected ---> drop state.";

        return false;
    }
}


bool LaiYangSnapshot::Private::verifyPrepost(const QJsonObject& content, QString& sender) const
{
    if (! content.contains(QLatin1String("receiver")) ||
        ! content.contains(QLatin1String("message")))
    {
        qWarning() << "Snapshot: prepost message has incorrect format";

        return false;
    }

    ACLMessage originalMessage(content[QLatin1String("message")].toString());

    VectorClock* messageClock = originalMessage.getTimeStamp();

    if (!messageClock)
    {
        qWarning() << "Snapshot: prepost message don't have clock --> drop.";

        return false;
    }

   sender = messageClock->getSiteID();

    // verify pre-message, avoid post-post or post-pre for a conherent snapshot

    if (states.contains(sender))
    {
        QJsonObject jsonClock = states[sender];

        jsonClock.remove(QLatin1String("state"));

        VectorClock* senderClock = new VectorClock(jsonClock);

        // if clock of sender at the moment of taking snapshot is smaller than clock of the message ==> it's not a pre-message
        if ((*senderClock) < (*messageClock))
        {
            qWarning() << "Snapshot: not a pre-message --> drop";

            return false;
        }
    }

    return true;
}

bool LaiYangSnapshot::Private::allStateColltected() const
{
    if (states.size() == nbApp)
    {
        return true;
    }

    return false;
}

bool LaiYangSnapshot::Private::allPrepostCollected() const
{
    if (nbWaitPrepost == 0)
    {
        return true;
    }

    return false;
}

int LaiYangSnapshot::Private::nbCollectedPrepost() const
{
    int nbPrepost = 0;

    for (QHash<QString, QVector<QJsonObject> >::const_iterator iter  = prepostMessages.cbegin();
                                                               iter != prepostMessages.cend();
                                                               ++iter)
    {
        nbPrepost += iter.value().size();
    }

    return nbPrepost;
}

/* ----------------------------------------------------------------------- LaiYangSnapshot main functions --------------------------------------------------------------------------------------*/

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

    if (!d->recorded)
    {
        requestSnapshot();
    }
}


void LaiYangSnapshot::colorMessage(QJsonObject& messageContent)
{
    // append color field to the content of the message
    messageContent[QLatin1String("snapshotted")] = d->recorded;

    // broadcast ---> increment message by nb of neighbor
    d->msgCounter += d->nbNeighbor;
}

bool LaiYangSnapshot::getColor(QJsonObject& messageContent)
{
    bool snapshotted = messageContent[QLatin1String("snapshotted")].toBool();
    messageContent.remove(QLatin1String("snapshotted"));

    if (snapshotted && !d->recorded)
    {
        requestSnapshot();
    }
    else
    {
        // decrement counter by 1
        --(d->msgCounter);

        if (!snapshotted && d->recorded)
        {
            // prepost detected
            return true;
        }
    }

    return false;
}

bool LaiYangSnapshot::processStateMessage(ACLMessage& message, bool fromLocal)
{
    VectorClock* timestamp = message.getTimeStamp();

    if (!timestamp)
    {
        qWarning() << "INFORM_STATE message doesn't contain timestamp.";

        return false;
    }

    QJsonObject state = timestamp->convertToJson();

    QJsonObject content = message.getContent();

    if (d->initiator)
    {
        if (content.contains(QLatin1String("msgCounter")))
        {
            // NOTE : only the first state message from a site has this field, in order to avoid redundance
            d->nbWaitPrepost += content[QLatin1String("msgCounter")].toInt();

            content.remove(QLatin1String("msgCounter"));
        }

        state[QLatin1String("state")] = content;

        if (d->collectState(state))
        {
           if (d->allStateColltected() && d->allPrepostCollected())
           {
               qDebug() << "Snapshot finish";

               d->recorded = false;
           }
        }

        return false;
    }
    else if (fromLocal)
    {
        state[QLatin1String("state")] = content;

        // Record State from local application
        if (d->collectState(state))
        {
            if (d->states.size() == 1)
            {
                // attach msgCounter of local site only to the first state message
                content[QLatin1String("msgCounter")] = d->nbWaitPrepost;
                message.setContent(content);
            }
        }
        else
        {
            return false;
        }
    }

    // Forward to initiator
    return true;
}



bool LaiYangSnapshot::processPrePostMessage(ACLMessage& message)
{
    QJsonObject prepostContent = message.getContent();

    QString sender;

    if (d->initiator)
    {
        if (d->verifyPrepost(prepostContent, sender))
        {
            d->prepostMessages[sender].append(prepostContent);

            --(d->nbWaitPrepost);

            qDebug() << "Initiator receives prepost : " << QJsonDocument(prepostContent).toJson(QJsonDocument::Compact);

            if (d->allStateColltected() && d->allPrepostCollected())
            {
                qDebug() << "Snapshot finish";

                d->recorded = false;
            }
        }

        return false;
    }

    return true;
}

void LaiYangSnapshot::requestSnapshot()
{
    d->recorded = true;

    d->nbWaitPrepost = d->msgCounter;

    // Chandy-Lamport marker
    ACLMessage* marker = new ACLMessage(ACLMessage::REQUEST_SNAPSHOT);

    emit signalRequestSnapshot(marker);
}

void LaiYangSnapshot::setNbOfApp(int nbApp)
{
    d->nbApp = nbApp;
}

void LaiYangSnapshot::setNbOfNeighbor(int nbNeighbor)
{
    d->nbNeighbor = nbNeighbor;
}

}
