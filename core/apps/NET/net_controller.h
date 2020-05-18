/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-22
 * Description : controller of NET application
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

    void init(const QCoreApplication &app) override;

    void takeSnapshot() const;

private:

    // main notification handler
    Q_SLOT void slotReceiveMessage(Header, Message) override;

private:

    class Private;
    Private* d;
};

}
#endif // NET_CONTROLLER_H
