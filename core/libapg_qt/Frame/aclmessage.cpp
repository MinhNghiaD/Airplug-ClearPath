#include "aclmessage.h"

namespace AirPlug
{

const QString ACLMessage::REQUEST          = QLatin1String("request");
const QString ACLMessage::INFORM           = QLatin1String("inform");
const QString ACLMessage::QUERY_IF         = QLatin1String("query_if");
const QString ACLMessage::REFUSE           = QLatin1String("refuse");
const QString ACLMessage::CONFIRM          = QLatin1String("confirm");
const QString ACLMessage::REQUEST_SNAPSHOT = QLatin1String("request_snapshot");
const QString ACLMessage::INFORM_STATE     = QLatin1String("inform_state");
const QString ACLMessage::PREPOST_MESSAGE  = QLatin1String("prepost_message");

ACLMessage::ACLMessage()
{

}

}
