/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-4
 * Description : implementation of network watch dog for network connectivity
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef WATCHDOG_H
#define WATCHDOG_H

// Qt include
#include <QObject>
#include <QDateTime>

// libapg include
#include "aclmessage.h"

namespace AirPlug
{

class Watchdog : public QObject
{
    Q_OBJECT
public:

    Watchdog();
    ~Watchdog();

    void receivePong(bool newApp);
    void receiveNetworkInfo(const ACLMessage& info);

private:

    void broadcastInfo();
    void requestInfo();

private:

    Q_SLOT void slotUpdateNbApp();
    Q_SLOT void eliminateDeprecatedInfo();

public:

    Q_SIGNAL void signalNbAppChanged(int nbApp);
    Q_SIGNAL void signalPingLocalApps();
    Q_SIGNAL void signalSendInfo(const ACLMessage&);

private:

    class Private;
    Private* d;
};


class SiteInfo
{
public:

    SiteInfo()
        : nbApp(0),
          lastUpdate(QDateTime::currentMSecsSinceEpoch())
    {
    }

    SiteInfo(const QString& siteID, int nbApp)
        : siteID(siteID),
          nbApp(nbApp),
          lastUpdate(QDateTime::currentMSecsSinceEpoch())
    {
    }

    void setNbApp(int nb)
    {
        nbApp      = nb;
        lastUpdate = QDateTime::currentMSecsSinceEpoch();
    }

public:

    QString siteID;
    int     nbApp;
    qint64  lastUpdate;
};

}
#endif // WATCHDOG_H
