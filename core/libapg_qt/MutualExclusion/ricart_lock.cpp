#include "ricart_lock.h"


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

    VectorClock* clock;
};

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

void RicartLock::request(const VectorClock& clock)
{
    if (d->clock != nullptr)
    {
        // already have a pending request  --> ignore
        return;
    }

    d->clock = new VectorClock(clock);

    // broadcast request

    ACLMessage message(ACLMessage::REQUEST_MUTEX);

    message.setTimeStamp(*(d->clock));

    emit signalRequest(message);
}

void RicartLock::receivePermission()
{

}

}
