#include "watchdog.h"

// Qt include
#include <QTimer>

namespace AirPlug
{

class Q_DECL_HIDDEN WatchDog::Private
{
public:

    Private()
        : nbLocalApp(0),
          temporaryNbApp(0)
    {
    }

    ~Private()
    {
    }

public:

    int nbApps() const;

public:

    int nbLocalApp;
    int temporaryNbApp;

};

int WatchDog::Private::nbApps() const
{
    // TODO count all nb of apps
    return nbLocalApp;
}

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

void WatchDog::receivePong()
{
    ++d->temporaryNbApp;
}

void WatchDog::slotUpdateNbApp()
{
    if (d->temporaryNbApp != d->nbLocalApp)
    {
        d->nbLocalApp = d->temporaryNbApp;

        emit signalNbAppChanged(d->nbApps());
    }

    d->temporaryNbApp = 0;

    // broadcast Info to all the watchdog
    ACLMessage  message(ACLMessage::UPDATE_ACTIVE);
    QJsonObject contents;
    contents[QLatin1String("nbApp")] = d->nbApps();
    message.setContent(contents);

    emit signalSendInfo(message);

    emit signalPingLocalApps();

    // reactivate timeout timer of 3s
    QTimer::singleShot(3000, this, SLOT(slotUpdateNbApp()));
}

}
