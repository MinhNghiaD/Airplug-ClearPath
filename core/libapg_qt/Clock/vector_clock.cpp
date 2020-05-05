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
    d->siteID     = siteID;
    d->localClock = vector;

    d->localClock.detach();
}

VectorClock::VectorClock(const QJsonObject& json)
    : d(new Private())
{
    d->siteID        = json[QLatin1String("siteID")].toString();
    QJsonArray clock = json[QLatin1String("clock")].toArray();

    for (int i = 0; i < clock.size(); ++i)
    {
        QJsonObject object = clock[i].toObject();

        d->localClock.insert(object.constBegin().key(), object.constBegin().value().toInt());
    }
}

VectorClock::VectorClock(const VectorClock& other)
    : d(new Private())
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

    return VectorClock(*this);
}

VectorClock VectorClock::operator++ (int)
{
    VectorClock clock = VectorClock(*this);

    ++(d->localClock[d->siteID]);

    return clock;
}

void VectorClock::updateClock(const VectorClock& other)
{
    // increment local clock
    ++(d->localClock[d->siteID]);

    // update other clocks
    for (QHash<QString, int>::const_iterator iter  = other.d->localClock.cbegin();
                                             iter != other.d->localClock.cend();
                                           ++iter)
    {
        if (d->siteID != iter.key())
        {
            if ( (! d->localClock.contains(iter.key()))      ||
                 (d->localClock[iter.key()] < iter.value()) )
            {
                d->localClock[iter.key()] = iter.value();
            }
        }
    }
}

bool VectorClock::operator < (const VectorClock& other) const
{
    for (QHash<QString, int>::const_iterator iter  = d->localClock.cbegin();
                                             iter != d->localClock.cend();
                                           ++iter)
    {
        if ( (! other.d->localClock.contains(iter.key()) && iter.value() != 0) ||
             (iter.value() > other.d->localClock[iter.key()]) )
        {
            return false;
        }
    }

    return true;
}

QJsonObject VectorClock::convertToJson() const
{
    QJsonObject json;

    json[QLatin1String("siteID")] = d->siteID;

    QJsonArray clock;

    for (QHash<QString, int>::const_iterator iter  = d->localClock.cbegin();
                                             iter != d->localClock.cend();
                                           ++iter)
    {
        QJsonObject localClock;
        localClock[iter.key()] = iter.value();

        clock.append(localClock);
    }

    json["clock"] = clock;

    return json;
}

QString VectorClock::getSiteID() const
{
    return d->siteID;
}

int VectorClock::getValue(const QString& siteID) const
{
    if (d->localClock.contains(siteID))
    {
        return d->localClock[siteID];
    }

    return 0;
}

QStringList VectorClock::siteLists() const
{
    return d->localClock.keys();
}

int VectorClock::sum() const
{
    int sum = 0;

    for (QHash<QString, int>::const_iterator iter  = d->localClock.cbegin();
                                             iter != d->localClock.cend();
                                           ++iter)
    {
        sum += iter.value();
    }

    return sum;
}

bool VectorClock::isGeneralSmallerThan(const VectorClock& other)
{
    if (sum() < other.sum())
    {
        return true;
    }
    else if ( (sum()      == other.sum())        &&
              (getSiteID() < other.getSiteID()) )
    {
        // in case of 2 clocks are independent  => compare siteID lexically
        return true;
    }

    return false;
}

}
