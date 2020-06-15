#include "watchdog.h"

// Qt include
#include <QTimer>
#include <QHash>
#include <QDebug>

namespace AirPlug
{
#define INTERVAL 500

class Q_DECL_HIDDEN Watchdog::Private
{
public:

    Private()
        : temporaryNbApp(0),
          nbTotalApp(0),
          nbSites(1)
    {
        timer.setInterval(INTERVAL);
    }

    ~Private()
    {
    }

public:

    int  nbApps() const;
    bool containDeprecatedInfo() const;

public:

    SiteInfo localInfo;
    int temporaryNbApp;
    QTimer timer;

    int nbTotalApp;
    int nbSites;

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
        if ((currentTime - iter.value().lastUpdate) >= INTERVAL)
        {
            return true;
        }
    }

    return false;
}

/* ----------------------------------------------------------------------- Watchdog main functions --------------------------------------------------------------------------------------*/


Watchdog::Watchdog(const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Watchdog"));
    d->localInfo = SiteInfo(siteID, 0);

    connect(&d->timer, &QTimer::timeout,
            this,      &Watchdog::slotCheckInfo, Qt::DirectConnection);

    d->timer.start();
}

Watchdog::~Watchdog()
{
    delete d;
}

void Watchdog::slotCheckInfo()
{
    // scan through info
    if (d->containDeprecatedInfo())
    {
        // Ping all Node in the network to check if they are alive
        requestInfo();

        eliminateDeprecatedInfo();
    }

    //qDebug() << d->localInfo.siteID << "nb of local app:" << d->temporaryNbApp;

    d->localInfo.nbApp = d->temporaryNbApp;
    d->temporaryNbApp  = 0;

    int newNbApps  = d->nbApps();
    int newNbSites = 1 + d->neighborsInfo.size();

    if (d->nbTotalApp != newNbApps || d->nbSites != newNbSites)
    {
        d->nbTotalApp = newNbApps;
        d->nbSites    = newNbSites;

        emit signalNetworkChanged(d->nbSites, d->nbTotalApp);
    }

    // broadcast Info to all Watchdogs
    broadcastInfo();

    // Ping local apps
    ACLMessage ping(ACLMessage::PING);
    emit signalPingLocalApps(ping);
}

void Watchdog::receivePong(bool newApp)
{
    // NOTE: all base applications have to send a PONG to NET after its initialization, in order to register,
    // All Site has to be notified in order to synchronize the increasing of number of apps before checking some termination conditions like Snapshot and Mutex
    // Since the channels are FIFO, therefore this information will be update at all sites before any message exchange
    if (newApp)
    {
        //qDebug() << d->localInfo.siteID << "new app enter";
    }

    //qDebug() << d->localInfo.siteID << "receive pong from local app";

    ++(d->temporaryNbApp);
}

void Watchdog::broadcastInfo()
{
    ACLMessage  message(ACLMessage::PONG);
    QJsonObject contents;

    contents[QLatin1String("nbApp")] = d->localInfo.nbApp;
    message.setContent(contents);

    emit signalSendInfo(message);

    //qDebug() << d->localInfo.siteID << "send info, nbApp =" << d->localInfo.nbApp;
}


void Watchdog::requestInfo()
{
    ACLMessage message(ACLMessage::PING);

    emit signalSendInfo(message);
}


bool Watchdog::eliminateDeprecatedInfo()
{
    qint64 currentTime     = QDateTime::currentMSecsSinceEpoch();
    bool containDeprecated = false;

    QHash<QString, SiteInfo>::iterator iter = d->neighborsInfo.begin();

    while (iter != d->neighborsInfo.end())
    {
        // deadline to beconsidered as deprecated is 2*INTERVAL
        // However it will not hurt the performance of other control algorithms that are waiting for response,
        // thank to the help of the notification signalNetworkChanged
        if ((currentTime - iter.value().lastUpdate) > INTERVAL*2)
        {
            containDeprecated = true;

            qDebug() << d->localInfo.siteID << "eliminate deprecated info : " << iter.key();

            iter = d->neighborsInfo.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    return containDeprecated;
}

void Watchdog::receiveNetworkInfo(const ACLMessage& info)
{
    QString neighborID = info.getSender();
    int     nbApp      = info.getContent()[QLatin1String("nbApp")].toInt();

    //qDebug() << d->localInfo.siteID << "receive network infor from" << neighborID << "with nb App" << info.getContent();

    d->neighborsInfo[neighborID].siteID     = neighborID;
    d->neighborsInfo[neighborID].nbApp      = nbApp;
    d->neighborsInfo[neighborID].lastUpdate = QDateTime::currentMSecsSinceEpoch();
}

}
