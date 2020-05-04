#include "watchdog.h"

// Qt include
#include <QTimer>
#include <QHash>

namespace AirPlug
{

class Q_DECL_HIDDEN WatchDog::Private
{
public:

    Private()
        : temporaryNbApp(0)
    {
    }

    ~Private()
    {
    }

public:

    int nbApps() const;

public:

    SiteInfo localInfo;
    int temporaryNbApp;

    QHash<QString, SiteInfo> neighborsInfo;

};

int WatchDog::Private::nbApps() const
{
    int totalNb = localInfo.nbApp;

    for (QHash<QString, SiteInfo>::const_iterator iter  = neighborsInfo.cbegin();
                                                  iter != neighborsInfo.cend();
                                                ++iter)
    {
        totalNb += iter.value().nbApp;
    }

    return totalNb;
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

void WatchDog::receivePong(bool newApp)
{
    // NOTE: all base applications have to send a PONG to NET after its initialization, in order to register,
    // All Site has to be notified in order to synchronize the increasing of number of apps before checking some termination conditions like Snapshot and Mutex
    if (newApp)
    {
        ++(d->localInfo.nbApp);

        emit signalNbAppChanged(d->nbApps());
        broadcastInfo();
    }

    ++d->temporaryNbApp;
}

void WatchDog::broadcastInfo()
{
    ACLMessage  message(ACLMessage::UPDATE_ACTIVE);
    QJsonObject contents;
    contents[QLatin1String("nbApp")] = d->nbApps();
    message.setContent(contents);

    emit signalSendInfo(message);
}

void WatchDog::slotUpdateNbApp()
{
    if (d->temporaryNbApp != d->localInfo.nbApp)
    {
        d->localInfo.nbApp = d->temporaryNbApp;

        emit signalNbAppChanged(d->nbApps());
    }

    d->temporaryNbApp = 0;

    // broadcast Info to all watchdogs
    broadcastInfo();

    emit signalPingLocalApps();

    // reactivate timeout timer of 3s
    QTimer::singleShot(3000, this, SLOT(slotUpdateNbApp()));
}

void WatchDog::receiveNetworkInfo(const ACLMessage& info)
{
    QString neighborID = info.getSender();

    if (!(d->neighborsInfo.contains(neighborID)))
    {
         d->neighborsInfo[neighborID] = SiteInfo(neighborID, info.getContent()[QLatin1String("nbApp")].toInt());
    }
    else
    {
        d->neighborsInfo[neighborID].setNbApp(info.getContent()[QLatin1String("nbApp")].toInt());
    }

    emit signalNbAppChanged(d->nbApps());
}

}
