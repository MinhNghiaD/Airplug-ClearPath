#include "message_transporter.h"

namespace AirPlug
{

MessageTransporter::MessageTransporter()
    : QThread()
{
    setObjectName(QLatin1String("Protocol"));

    start();
}

MessageTransporter::~MessageTransporter()
{
}

}

