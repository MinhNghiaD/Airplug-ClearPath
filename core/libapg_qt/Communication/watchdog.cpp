#include "watchdog.h"

// Qt include
#include <QTimer>
#include <QHash>

namespace AirPlug
{

class Q_DECL_HIDDEN Watchdog::Private
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
    bool containDeprecatedInfo() const;

public:

    SiteInfo localInfo;
    int temporaryNbApp;

    QHash<QString, SiteInfo> neighborsInfo;
};

int Watchdog::Private::nbApps() const
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

bool Watchdog::Private::containDeprecatedInfo() const
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    for (QHash<QString, SiteInfo>::const_iterator iter  = neighborsInfo.cbegin();
                                                  iter != neighborsInfo.cend();
                                                ++iter)
    {
        // period between 2 update is about 3000 ms, therefore
        // deadline to beconsidered as deprecated is 4000 ms
        if ((currentTime - iter.value().lastUpdate) >= 4000)
        {
            return true;
        }
    }

    return false;
}



Watchdog::Watchdog(const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Watchdog"));
    d->localInfo = SiteInfo(siteID, 0);
}

Watchdog::~Watchdog()
{
    delete d;
}

void Watchdog::receivePong(bool newApp)
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

void Watchdog::broadcastInfo()
{
    ACLMessage  message(ACLMessage::UPDATE_ACTIVE);
    QJsonObject contents;
    contents[QLatin1String("nbApp")] = d->nbApps();
    message.setContent(contents);

    emit signalSendInfo(message);
}

void Watchdog::requestInfo()
{
    ACLMessage message(ACLMessage::PING);
    emit signalSendInfo(message);

    // deadline for responses is 2000 ms
    QTimer::singleShot(2000, this, SLOT(eliminateDeprecatedInfo()));
}

void Watchdog::slotUpdateNbApp()
{
    if (d->containDeprecatedInfo())
    {
        // Ping all Node in the network to check if they are alive
        requestInfo();
    }

    if (d->temporaryNbApp != d->localInfo.nbApp)
    {
        d->localInfo.nbApp = d->temporaryNbApp;

        emit signalNbAppChanged(d->nbApps());
    }

    d->temporaryNbApp = 0;

    // broadcast Info to all Watchdogs
    broadcastInfo();

    emit signalPingLocalApps();

    // reactivate timeout timer of 3s
    QTimer::singleShot(3000, this, SLOT(slotUpdateNbApp()));
}

void Watchdog::eliminateDeprecatedInfo()
{
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    bool containDeprecated = false;

    QHash<QString, SiteInfo>::iterator iter  = d->neighborsInfo.begin();

    while (iter != d->neighborsInfo.end())
    {
        // deadline to beconsidered as deprecated is 4000 ms
        if ((currentTime - iter.value().lastUpdate) >= 4000)
        {
            containDeprecated = true;

            d->neighborsInfo.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if (containDeprecated)
    {
        emit signalNbAppChanged(d->nbApps());
    }
}

void Watchdog::receiveNetworkInfo(const ACLMessage& info)
{
    QString neighborID = info.getSender();

    if (!(d->neighborsInfo.contains(neighborID)))
    {
         d->neighborsInfo[neighborID] = SiteInfo(neighborID, info.getContent()[QLatin1String("nbApp")].toInt());
    }
    else
    {
        int nbApp = info.getContent()[QLatin1String("nbApp")].toInt();

        if (d->neighborsInfo[neighborID].nbApp == nbApp)
        {
            return;
        }

        d->neighborsInfo[neighborID].setNbApp(nbApp);
    }

    emit signalNbAppChanged(d->nbApps());
}


}
