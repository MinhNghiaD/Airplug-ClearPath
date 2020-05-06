#include "laiyang_snapshot.h"

// Qt includes
#include <QFile>
#include <QDateTime>
#include <QDebug>

// libapg include
#include "vector_clock.h"

namespace AirPlug
{

class Q_DECL_HIDDEN LaiYangSnapshot::Private
{
public:

    Private()
        : status(READY),
          initiator(false),
          msgCounter(0),
          nbWaitPrepost(0),
          nbApp(0),
          nbNeighbor(0),
          nbReadyNeighbor(0)
    {
    }

    ~Private()
    {
    }

public:

    bool validateState(const QJsonObject& state) const;

    /**
     * A State object should have the form of :
     * {
     *     siteID : QString
     *     clock  : map <siteID, lamport clock>
     *     state  : {
     *                  options       : application option
     *                  local varable : jsonObject
     *              }
     * }
     */
    bool collectState (const QJsonObject& state);
    bool verifyPrepost(const QJsonObject& content, const QString& sender) const;

    bool allStateColltected()  const;
    bool allPrepostCollected() const;
    int  nbCollectedPrepost()  const;

public:

    Status status;
    bool   initiator;
    int    msgCounter;
    int    nbWaitPrepost;
    int    nbApp;
    int    nbNeighbor;
    int    nbReadyNeighbor;

    QHash<QString, QJsonObject>           states;
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
            qDebug() << "incoherent snapshot" << "new clock" << newClock.convertToJson() << "clock recorded" << currentClock.convertToJson();
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

// TODO verify this function
bool LaiYangSnapshot::Private::verifyPrepost(const QJsonObject& originalMessage, const QString& sender) const
{
    VectorClock messageClock(originalMessage[QLatin1String("timestamp")].toObject());

    // verify pre-message, avoid post-post or post-pre for a conherent snapshot

    if (states.contains(sender))
    {
        QJsonObject jsonClock = states[sender];

        jsonClock.remove(QLatin1String("state"));

        VectorClock* senderClock = new VectorClock(jsonClock);

        // if clock of sender at the moment of taking snapshot is smaller than clock of the message ==> it's not a pre-message
        if ((*senderClock) < (messageClock))
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
    setObjectName(QLatin1String("Snapshot"));
}

LaiYangSnapshot::~LaiYangSnapshot()
{
    delete d;
}

void LaiYangSnapshot::init()
{
    if (d->status == READY)
    {
        d->initiator = true;

        requestSnapshot();
    }
    else
    {
        qWarning() << "Snapshot is not ready";
    }
}


void LaiYangSnapshot::colorMessage(QJsonObject& messageContent, int nbReceivers)
{
    messageContent[QLatin1String("snapshotStatus")] = d->status;

    // nbReceivers == 0 => broadcast ---> increment message by nb of neighbor
    if (nbReceivers == 0)
    {
        nbReceivers = d->nbNeighbor;
    }

    d->msgCounter += d->nbNeighbor;
}

bool LaiYangSnapshot::getColor(QJsonObject& messageContent)
{
    if (! messageContent.contains(QLatin1String("snapshotStatus")))
    {
        return false;
    }

    Status snapshotStatus = static_cast<Status>(messageContent[QLatin1String("snapshotStatus")].toInt());

    messageContent.remove(QLatin1String("snapshotStatus"));

    // READY -> RECORDED transition
    if (snapshotStatus == RECORDED && d->status == READY)
    {
        requestSnapshot();
    }

    // decrement counter by 1
    --(d->msgCounter);

    if (snapshotStatus == READY && d->status == RECORDED)
    {
        // preopost detected
        return true;
    }

    // RECORDED -> RECOVERING transition
    if (snapshotStatus == RECOVERING && d->status == RECORDED)
    {
        d->status = RECOVERING;

        // forward message to inform initiator
        ACLMessage message(ACLMessage::SNAPSHOT_RECOVER);

        emit signalSendSnapshotMessage(message);
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

    QJsonObject state   = timestamp->convertToJson();
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
            // Terminate snapshot
            if (d->allStateColltected() && d->allPrepostCollected())
            {
                d->status = RECOVERING;
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

bool LaiYangSnapshot::processPrePostMessage(const ACLMessage& message)
{
    if (d->initiator)
    {
        QJsonObject originalMessage = message.getContent();
        QString     receiver        = message.getSender();
        QString     sender          = originalMessage[QLatin1String("sender")].toString();

        originalMessage[QLatin1String("receiver")] = receiver;

        if (d->verifyPrepost(originalMessage, sender))
        {
            --(d->nbWaitPrepost);

            d->prepostMessages[sender].append(originalMessage);

            if (d->allStateColltected() && d->allPrepostCollected())
            {
                // finish snapshot
                d->status = RECOVERING;
            }
        }

        return false;
    }

    // forward to initiator
    return true;
}

ACLMessage LaiYangSnapshot::encodePrepostMessage(const ACLMessage& message)
{
    ACLMessage prepost(ACLMessage::PREPOST_MESSAGE);

    // encapsulate original message into Prepost message content
    prepost.setContent(message.toJsonObject());

    return prepost;
}

bool LaiYangSnapshot::processRecoveringMessage(const ACLMessage& message)
{
    if (d->initiator && d->status == RECOVERING)
    {
        ++d->nbReadyNeighbor;

        if (d->nbReadyNeighbor == d->nbNeighbor)
        {
            finishSnapshot();

            d->nbReadyNeighbor = 0;
        }

        return false;
    }

    // Forward to initiator
    return true;
}

bool LaiYangSnapshot::processReadyMessage(const ACLMessage& message)
{
    if (d->initiator)
    {
        return false;
    }

    finishSnapshot();

    return true;
}

void LaiYangSnapshot::requestSnapshot()
{
    d->status        = RECORDED;
    d->nbWaitPrepost = d->msgCounter;

    // Chandy-Lamport marker
    ACLMessage marker(ACLMessage::REQUEST_SNAPSHOT);

    emit signalRequestSnapshot(marker);
}

void LaiYangSnapshot::finishSnapshot()
{
    if (d->initiator)
    {
        qDebug() << "Snapshot finish";
        // broadcast to all site to get ready for snapshot
        ACLMessage inform(ACLMessage::READY_SNAPSHOT);

        emit signalSendSnapshotMessage(inform);

        saveSnapshot();
    }

    d->status        = READY;
    d->initiator     = false;
    d->nbWaitPrepost = 0;

    d->states.clear();
    d->prepostMessages.clear();
}

void LaiYangSnapshot::setNbOfApp(int nbApp)
{
    d->nbApp = nbApp;
}

void LaiYangSnapshot::setNbOfNeighbor(int nbNeighbor)
{
    d->nbNeighbor = nbNeighbor;
}

void LaiYangSnapshot::saveSnapshot() const
{
    QFile file("snapshot.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        return;
    }

    QTextStream out(&file);

    out << "//////////////////////////////////////////////////////////////////// Snapshot at " << QDateTime::currentDateTime().toLocalTime().toString(QLatin1String("h:m:s ap"))
        <<"////////////////////////////////////////////////////////////////////////\n";

    out << " --------------------------------------------------------------------- States------------------------------------------------------------ : \n";
    for (QHash<QString, QJsonObject>::const_iterator iter  = d->states.cbegin();
                                                     iter != d->states.cend();
                                                     ++iter)
    {
        out << "+ app = " << iter.key() << " : \n";
        out << QJsonDocument(iter.value()).toJson() << "\n";
    }

    out << " ----------------------------------------------------------------- Prepost messages ------------------------------------------------------------ : \n";
    for (QHash<QString, QVector<QJsonObject> >::const_iterator iter  = d->prepostMessages.cbegin();
                                                               iter != d->prepostMessages.cend();
                                                               ++iter)
    {
        out << "+ sender = " << iter.key() << " : \n";

        for (int i = 0; i < iter.value().size(); ++i)
        {
            out << QJsonDocument(iter.value()[i]).toJson() << "\n";
        }
    }
}

}
