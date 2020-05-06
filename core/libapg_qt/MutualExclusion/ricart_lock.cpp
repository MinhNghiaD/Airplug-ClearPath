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
    QStringList  queue;
};

bool RicartLock::Private::isLessPriority(const VectorClock& requesterClock) const
{
    if (! clock)
    {
         return true;
    }

    if (clock->isGeneralSmallerThan(requesterClock))
    {
        return false;
    }

    return true;
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

    //qDebug() << d->clock->getSiteID() << "request mutex with clock" << d->clock->convertToJson();

    // broadcast request
    ACLMessage message(ACLMessage::REQUEST_MUTEX);

    message.setTimeStamp(*(d->clock));

    emit signalResponse(message);
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
    qDebug() << d->clock->getSiteID() << "Enter critical section";
    emit signalEnterRaceCondition();
}

void RicartLock::unlock()
{
    qDebug() << d->clock->getSiteID() << "Out of critical section, liberates pending applications: " << d->queue;

    if (! d->queue.isEmpty())
    {
        // give permission to all pending apps
        ACLMessage approval(ACLMessage::ACCEPT_MUTEX);

        QJsonObject content;
        content[QLatin1String("apps")] = getPendingQueue();

        approval.setContent(content);

        emit signalResponse(approval);
    }

    delete d->clock;

    d->clock = nullptr;
    d->queue.clear();
}

void RicartLock::restart()
{
    delete d->clock;

    d->clock = nullptr;
    d->queue.clear();
}

QJsonArray RicartLock::getPendingQueue() const
{
    return QJsonArray::fromStringList(d->queue);
}

}
