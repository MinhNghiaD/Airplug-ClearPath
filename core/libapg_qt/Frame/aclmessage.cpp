#include "aclmessage.h"

namespace AirPlug
{

const QString ACLMessage::REQUEST          = QLatin1String("request");
const QString ACLMessage::INFORM           = QLatin1String("inform");
const QString ACLMessage::QUERY_IF         = QLatin1String("query_if");
const QString ACLMessage::REFUSE           = QLatin1String("refuse");
const QString ACLMessage::CONFIRM          = QLatin1String("confirm");
const QString ACLMessage::UNKNOWN          = QLatin1String("unknow");
const QString ACLMessage::REQUEST_SNAPSHOT = QLatin1String("request_snapshot");
const QString ACLMessage::INFORM_STATE     = QLatin1String("inform_state");
const QString ACLMessage::PREPOST_MESSAGE  = QLatin1String("prepost_message");

ACLMessage::ACLMessage(const QString& perfomative)
    : Message()
{
    addContent(QLatin1String("perfomative"), perfomative);
}

ACLMessage::~ACLMessage()
{
}

void ACLMessage::setContent(const QJsonObject& content)
{
    addContent(QLatin1String("content"), QJsonDocument(content).toJson(QJsonDocument::Compact));
}

void ACLMessage::setTimeStamp(const VectorClock& clock)
{
    addContent(QLatin1String("timestamp"), QJsonDocument(clock.convertToJson()).toJson(QJsonDocument::Compact));
}

VectorClock* ACLMessage::getTimeStamp() const
{
    QHash<QString, QString> contents = getContents();

    if (contents.contains(QLatin1String("timestamp")))
    {
        QJsonObject jsonClock =  QJsonDocument::fromJson(contents[QLatin1String("timestamp")].toUtf8()).object();

        return new VectorClock(jsonClock);
    }

    return nullptr;
}

QJsonObject ACLMessage::getContent() const
{
    return QJsonDocument::fromJson(getContents()[QLatin1String("content")].toUtf8()).object();
}



}
