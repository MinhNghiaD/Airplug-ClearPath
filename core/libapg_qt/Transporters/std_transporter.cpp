#include "std_transporter.h"

//C++ include
#include <iostream>
#include <string>

//Qt include
#include <QSocketNotifier>
#include <QDebug>

namespace AirPlug
{

StdTransporter::StdTransporter()
    : MessageTransporter()
{
    QObject::setObjectName(QLatin1String("StandardIO Protocol"));
}

StdTransporter::~StdTransporter()
{
}

void StdTransporter::run()
{
    while(true)
    {
        std::string message;
        std::getline(std::cin, message);

        emit signalMessageReceived(QString::fromStdString(message));
        // slow down to avoid flooding the channel
        //QThread::msleep(10);
    }
}

void StdTransporter::send(const QString& message)
{
    std::cout << message.toStdString() << std::endl;
}

}
