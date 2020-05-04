#include "watchdog.h"

namespace AirPlug
{

class Q_DECL_HIDDEN WatchDog::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
    }

public:


};

WatchDog::WatchDog()
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Watchdog"));
}

WatchDog::~WatchDog()
{
    delete d;
}

}
