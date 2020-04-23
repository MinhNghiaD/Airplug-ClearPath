#include "vector_clock.h"

namespace AirPlug
{

class Q_DECL_HIDDEN VectorClock::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
    }

public:

    // because it's more difficult to keep track of site's order ==> use a Hash table to map siteID with its local clock
    QHash<QString, int> localClock;
    QString             siteID;
};

VectorClock::VectorClock(const QString& siteID)
    : d(new Private())
{
    d->siteID = siteID;

    d->localClock.insert(siteID, 0);
}

VectorClock::VectorClock(const QString& siteID, const QHash<QString, int>& vector)
    : d(new Private())
{
    d->siteID = siteID;

    d->localClock = vector;
    d->localClock.detach();
}

VectorClock::~VectorClock()
{
    delete d;
}

VectorClock VectorClock::operator++ ()
{
    ++(d->localClock[d->siteID]);

    return VectorClock(d->siteID, d->localClock);
}

VectorClock VectorClock::operator++ (int)
{
    VectorClock clock = VectorClock(d->siteID, d->localClock);

    ++(d->localClock[d->siteID]);

    return clock;
}

}
