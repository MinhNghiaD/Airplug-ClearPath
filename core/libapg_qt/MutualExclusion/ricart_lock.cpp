#include "ricart_lock.h"

#include <QDebug>

namespace AirPlug
{

class Q_DECL_HIDDEN RicartLock::Private
{
public:

    Private()
        : clock(nullptr)
    {
    }

    ~Private()
    {
        delete clock;
    }

public:

    bool isLessPriority(const VectorClock& requesterClock) const;

public:

    VectorClock* clock;
    QString      ID;                // TODO Remove this after debug
    // List of appID waiting for this Mutex
    QStringList  queue;
};

bool RicartLock::Private::isLessPriority(const VectorClock& requesterClock) const
{
    if (clock == nullptr || (requesterClock.sum() < clock->sum()))
    {
        if (!clock)
        {
            qDebug() << ID << "Approve" << requesterClock.getSiteID() << "because not having request";
        }
        else
        {
            qDebug() << ID << "Approve" << requesterClock.getSiteID() << "because" << ID << "clock :" << clock->convertToJson() << " > " << requesterClock.getSiteID() << "clock :" << requesterClock.convertToJson();
        }

        return true;
    }
    else if ( (requesterClock.sum()      == clock->sum())        &&
              (requesterClock.getSiteID() < clock->getSiteID()) )
    {
        // in case of 2 clocks are independent  => compare appID lexically
        qDebug() << ID << "add" << requesterClock.getSiteID() << "Approve, because" << ID << "clock :" << clock->convertToJson() << " > " << requesterClock.getSiteID() << "clock :" << requesterClock.convertToJson();

        return true;
    }

    qDebug() << ID << "add" << requesterClock.getSiteID() << "to pending queue, because" << ID << "clock :" << clock->convertToJson() << " < " << requesterClock.getSiteID() << "clock :" << requesterClock.convertToJson();
    return false; 
}

RicartLock::RicartLock()
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Ricart Lock"));
}

RicartLock::~RicartLock()
{
    delete d;
}

void RicartLock::trylock(const VectorClock& requesterClock)
{
    if (d->clock != nullptr)
    {
        // already have a pending request  --> ignore
        return;
    }

    d->clock = new VectorClock(requesterClock);
    d->ID    = d->clock->getSiteID();

    qDebug() << d->clock->getSiteID() << "request mutex with clock" << d->clock->convertToJson();

    // broadcast request

    ACLMessage message(ACLMessage::REQUEST_MUTEX);

    message.setTimeStamp(*(d->clock));

    emit signalResponse(message);

    //qDebug() << requesterClock.getSiteID() << "try lock";
}

void RicartLock::receiveExternalRequest(const VectorClock& requesterClock)
{
    if (d->isLessPriority(requesterClock))
    {
        ACLMessage approval(ACLMessage::ACCEPT_MUTEX);

        QJsonArray apps;
        apps.append(requesterClock.getSiteID());

        QJsonObject content;
        content[QLatin1String("apps")] = apps;

        approval.setContent(content);

        emit signalResponse(approval);
    }
    else
    {
        // Not approve  => add to pending queue
        d->queue.append(requesterClock.getSiteID());
    }
}

void RicartLock::lock()
{
    qDebug() << d->clock->getSiteID() << "Enter race condition";
    emit signalEnterRaceCondition();
}

void RicartLock::unlock()
{
    qDebug() << d->clock->getSiteID() << "Out of race condition, liberate pending" << d->queue;

    if (! d->queue.isEmpty())
    {
        // give permission to all pending apps
        ACLMessage approval(ACLMessage::ACCEPT_MUTEX);

        QJsonArray apps = QJsonArray::fromStringList(d->queue);

        QJsonObject content;
        content[QLatin1String("apps")] = apps;

        approval.setContent(content);

        emit signalResponse(approval);
    }

    //qDebug() << d->clock->getSiteID() << "Out of race condition";

    delete d->clock;

    d->clock = nullptr;
    d->queue.clear();
}

}
