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

VectorClock::VectorClock(const VectorClock& other)
{
    d->siteID     = other.d->siteID;

    d->localClock = other.d->localClock;
    d->localClock.detach();
}

VectorClock::~VectorClock()
{
    delete d;
}

VectorClock VectorClock::operator= (const VectorClock& other)
{
    d->siteID     = other.d->siteID;

    d->localClock = other.d->localClock;
    d->localClock.detach();
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

void VectorClock::updateClock(const VectorClock& other)
{
    // increment local clock
    ++(d->localClock[d->siteID]);

    // update other clocks
    for (QHash<QString, int>::const_iterator iter = other.d->localClock.cbegin();
         iter != other.d->localClock.cend();
         ++iter)
    {
        if (d->siteID != iter.key())
        {
            if (! d->localClock.contains(iter.key()))
            {
                d->localClock.insert(iter.key(), iter.value());
            }
            else if (d->localClock[iter.key()] < iter.value())
            {
                d->localClock[iter.key()] = iter.value();
            }
        }
    }
}

}
