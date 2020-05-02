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
    // List of appID waiting for this Mutex
    QStringList  queue;
};

bool RicartLock::Private::isLessPriority(const VectorClock& requesterClock) const
{
    if (clock == nullptr || (requesterClock < (*clock)))
    {
        return true;
    }
    else if (! ((*clock) < requesterClock) && (requesterClock.getSiteID() < clock->getSiteID()))
    {
        // in case of 2 clocks are independent  => compare appID lexically
        return true;
    }

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

    // broadcast request

    ACLMessage message(ACLMessage::REQUEST_MUTEX);

    message.setTimeStamp(*(d->clock));

    emit signalResponse(message);

    qDebug() << requesterClock.getSiteID() << "try lock";
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
    emit signalEnterRaceCondition();
}

void RicartLock::unlock()
{
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

    qDebug() << d->clock->getSiteID() << "Out of race condition";

    delete d->clock;

    d->clock = nullptr;
    d->queue.clear();
}

}
