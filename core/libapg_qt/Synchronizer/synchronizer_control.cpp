#include "synchronizer_control.h"
namespace AirPlug
{
class SynchronizerControl::Private
{
public:

    explicit Private()
        : nbApps(0)
    {
    }

    ~Private()
    {
    }

public:

    // TODO sync on Base application level
    QString initator;
    int     nbApps;
    int     nbWaitMsg;
    int     nbWaitAck;
};

SynchronizerControl::SynchronizerControl()
    : QObject(),
      d(new Private())
{
}

SynchronizerControl::~SynchronizerControl()
{
    delete d;
}

void SynchronizerControl::init(const QString& initiator)
{
    // TODO SYNCHRONIZER 1 : set initiator and, nbWaitMsg and nbWaitAck to nbApps

}

void SynchronizerControl::setNbApps(int nbApps)
{
    d->nbApps = nbApps;
}

}
