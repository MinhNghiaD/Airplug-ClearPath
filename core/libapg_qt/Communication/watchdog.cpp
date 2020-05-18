#include "watchdog.h"

// Qt include
#include <QTimer>
#include <QHash>
#include <QDebug>

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

    int  nbApps() const;
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

/* ----------------------------------------------------------------------- Watchdog main functions --------------------------------------------------------------------------------------*/


Watchdog::Watchdog(const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Watchdog"));
    d->localInfo = SiteInfo(siteID, 0);

    QTimer::singleShot(3000, this, SLOT(slotUpdateNbApp()));
}

Watchdog::~Watchdog()
{
    delete d;
}

void Watchdog::receivePong(bool newApp)
{
    // NOTE: all base applications have to send a PONG to NET after its initialization, in order to register,
    // All Site has to be notified in order to synchronize the increasing of number of apps before checking some termination conditions like Snapshot and Mutex
    // Since the channels are FIFO, therefore this information will be update at all sites before any message exchange   
    if (newApp)
    {
        //++(d->localInfo.nbApp);
        qDebug() << d->localInfo.siteID << "new app enter";
        //emit signalNetworkChanged((1 + d->neighborsInfo.size()), d->nbApps());
        //broadcastInfo();
    }

    ++d->temporaryNbApp;
}

void Watchdog::broadcastInfo()
{
    ACLMessage  message(ACLMessage::PONG);
    QJsonObject contents;

    contents[QLatin1String("nbApp")] = d->localInfo.nbApp;
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
    // TODO: stablize this functionality, a delayed pong can cause the problem, can vector clock help???s
    if (d->containDeprecatedInfo())
    {
        // Ping all Node in the network to check if they are alive
        requestInfo();
    }

    if (d->temporaryNbApp != d->localInfo.nbApp)
    {
        //qDebug() << d->localInfo.siteID << "nb of local app change, from" << d->localInfo.nbApp << "to" << d->temporaryNbApp;
        d->localInfo.nbApp = d->temporaryNbApp;

        emit signalNetworkChanged((1 + d->neighborsInfo.size()), d->nbApps());
    }

    d->temporaryNbApp = 0;

    // broadcast Info to all Watchdogs
    broadcastInfo();

    // TODO: polling doesn't scale, find another method that does not flood the network
/*
    ACLMessage ping(ACLMessage::PING);

    emit signalPingLocalApps(ping);

    // reactivate timeout timer of 3s
    QTimer::singleShot(3000, this, SLOT(slotUpdateNbApp()));
*/
}

void Watchdog::eliminateDeprecatedInfo()
{
    qint64 currentTime     = QDateTime::currentMSecsSinceEpoch();
    bool containDeprecated = false;

    QHash<QString, SiteInfo>::iterator iter = d->neighborsInfo.begin();

    while (iter != d->neighborsInfo.end())
    {
        // deadline to beconsidered as deprecated is 4000 ms
        // NOTE : the period since a site went down until it is eliminated is minimum (2000 + 4000) ms
        // However it will not hurt the performance of other control algorithms that are waiting for response, thank to the help of the notification signalNetworkChanged
        if ((currentTime - iter.value().lastUpdate) >= 4000)
        {
            containDeprecated = true;

            //qDebug() << d->localInfo.siteID << "eliminate deprecated info : " << iter.key();

            iter = d->neighborsInfo.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if (containDeprecated)
    {
        //qDebug() << d->localInfo.siteID << "remove deprecated, total nb app rests" << d->nbApps();
        emit signalNetworkChanged((1 + d->neighborsInfo.size()), d->nbApps());
    }
}

void Watchdog::receiveNetworkInfo(const ACLMessage& info)
{
    QString neighborID = info.getSender();

    //qDebug() << d->localInfo.siteID << "receive network infor from" << neighborID << "with nb App" << info.getContent();

    if (!(d->neighborsInfo.contains(neighborID)))
    {
         d->neighborsInfo[neighborID] = SiteInfo(neighborID, info.getContent()[QLatin1String("nbApp")].toInt());

         if (d->neighborsInfo[neighborID].nbApp == 0)
         {
             return;
         }
    }
    else
    {
        int nbApp = info.getContent()[QLatin1String("nbApp")].toInt();

        if (d->neighborsInfo[neighborID].nbApp == nbApp)
        {
            d->neighborsInfo[neighborID].lastUpdate = QDateTime::currentMSecsSinceEpoch();

            return;
        }
        else
        {
            d->neighborsInfo[neighborID].setNbApp(nbApp);
        }
    }

    //qDebug() << d->localInfo.siteID << "nb of total app change, new value" << d->nbApps();

    emit signalNetworkChanged((1 + d->neighborsInfo.size()), d->nbApps());
}


}
