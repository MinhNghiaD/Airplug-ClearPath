#include "aclmessage.h"

namespace AirPlug
{

ACLMessage::ACLMessage(Performative perfomative)
    : Message()
{
    addContent(QLatin1String("perfomative"), QString::number(perfomative));
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

ACLMessage::Performative ACLMessage::getPerformative() const
{
    return static_cast<Performative>(getContents()[QLatin1String("perfomative")].toInt());
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
