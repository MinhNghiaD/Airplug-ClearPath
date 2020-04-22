/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : controller of BAS application
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef NET_CONTROLLER_H
#define NET_CONTROLLER_H

// Qt include
#include <QObject>

// Local include
#include "application_controller.h"

using namespace AirPlug;

namespace NetApplication
{

class NetController: public ApplicationController
{
    Q_OBJECT
public:

    NetController(QObject* parent = nullptr);
    ~NetController();

    // Initialization of program
    //void init(const QCoreApplication& app) override;

    //void pause(bool b);

    //void setMessage(const QString& msg);

public:

    //Q_SLOT void slotActivateTimer(int period);
    //Q_SLOT void slotDeactivateTimer();
    //Q_SLOT void slotPeriodChanged(int period);
    //Q_SLOT void slotSendMessage();

signals:

    //Q_SIGNAL void signalSequenceChange(int);

    //Q_SIGNAL void signalMessageReceived(Header, Message);

private:

    // main notification handler
    Q_SLOT void slotReceiveMessage(Header, Message) override;

private:

    class Private;
    Private* d;
};

}
#endif // NET_CONTROLLER_H
