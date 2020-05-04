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

#include <QObject>

#include "aclmessage.h"

namespace AirPlug
{

class WatchDog : public QObject
{
    Q_OBJECT
public:

    WatchDog();
    ~WatchDog();

    void receivePong();

public:

    Q_SIGNAL void signalNbAppChanged(int nbApp);
    Q_SIGNAL void signalPingLocalApps();
    Q_SIGNAL void signalSendInfo(const ACLMessage&);

private:

    Q_SLOT void slotUpdateNbApp();

private:

    class Private;
    Private* d;
};

}
#endif // WATCHDOG_H
