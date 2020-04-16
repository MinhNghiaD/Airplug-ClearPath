/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : UDP transport layer
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef UDP_TRANSPORTER_H
#define UDP_TRANSPORTER_H

//local include
#include "message_transporter.h"

namespace AirPlug
{

class UdpTransporter : public MessageTransporter
{
    Q_OBJECT
public:

    UdpTransporter(const QString& host   = QLatin1String("0.0.0.0"),
                   int            port   = 7777,
                   UdpType        type   = UniCast,
                   QObject*       parent = nullptr);

    ~UdpTransporter();

    void send(const QString& message) override;

private:

    Q_SLOT void slotMessageArrive();

private:

    class Private;
    Private* d;
};

}

#endif // UDP_TRANSPORTER_H
