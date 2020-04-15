#ifndef STD_TRANSPORTER_H
#define STD_TRANSPORTER_H

#include "message_transporter.h"

namespace AirPlug
{

class StdTransporter : public MessageTransporter
{
    Q_OBJECT

public:

    StdTransporter(QObject* parent = nullptr);
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
