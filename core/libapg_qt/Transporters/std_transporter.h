/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : stdio transport layer
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef STD_TRANSPORTER_H
#define STD_TRANSPORTER_H

#include "message_transporter.h"

namespace AirPlug
{

class StdTransporter : public MessageTransporter
{
    Q_OBJECT

public:

    explicit StdTransporter(QObject* parent = nullptr);
    ~StdTransporter();

    void send(const QString& message) override;

private:

    Q_SLOT void slotMessageArrive();

private:

    class Private;
    Private* d;
};

}
#endif // STD_TRANSPORTER_H
