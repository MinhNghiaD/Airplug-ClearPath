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

#include <QThread>

#include "message_transporter.h"

namespace AirPlug
{

class StdTransporter : public MessageTransporter
{
    Q_OBJECT

public:

    explicit StdTransporter();
    ~StdTransporter();

    void send(const QString& message) override;

private:

    void run() override;
};

}
#endif // STD_TRANSPORTER_H
