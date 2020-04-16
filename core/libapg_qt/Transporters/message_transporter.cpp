#include "message_transporter.h"

namespace AirPlug
{

MessageTransporter::MessageTransporter(QObject* parent)
    : QObject(parent)
{
    setObjectName(QLatin1String("Protocol"));
}

MessageTransporter::~MessageTransporter()
{
}

}

