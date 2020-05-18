#include "aclmessage.h"

namespace AirPlug
{

ACLMessage::ACLMessage(Performative performative)
    : Message()
{
    addContent(QLatin1String("perfomative"), QString::number(performative));
}

ACLMessage::ACLMessage(const QString& message)
    : Message(message)
{
}

ACLMessage::~ACLMessage()
{
}

void ACLMessage::setPerformative(Performative performative)
{
    addContent(QLatin1String("perfomative"), QString::number(performative));
}

void ACLMessage::setContent(const QJsonObject& content)
{
    addContent(QLatin1String("content"), QJsonDocument(content).toJson(QJsonDocument::Compact));
}

void ACLMessage::setTimeStamp(const VectorClock& clock)
{
    addContent(QLatin1String("timestamp"), QJsonDocument(clock.convertToJson()).toJson(QJsonDocument::Compact));
}

void ACLMessage::setSender(const QString& siteID)
{
    addContent(QLatin1String("sender"), siteID);
}

void ACLMessage::setNbSequence(int nbSequence)
{
    addContent(QLatin1String("nbSequence"), QString::number(nbSequence));
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

QString ACLMessage::getSender() const
{
    return getContents()[QLatin1String("sender")];
}

int ACLMessage::getNbSequence() const
{
    return getContents()[QLatin1String("nbSequence")].toInt();
}

QJsonObject ACLMessage::toJsonObject() const
{
    QHash<QString, QString> contents = getContents();

    QJsonObject json;

    json[QLatin1String("perfomative")] = contents[QLatin1String("perfomative")];
    json[QLatin1String("sender")]      = contents[QLatin1String("sender")];
    json[QLatin1String("nbSequence")]  = contents[QLatin1String("nbSequence")].toInt();

    if (contents.contains(QLatin1String("timestamp")))
    {
        json[QLatin1String("timestamp")] = QJsonDocument::fromJson(contents[QLatin1String("timestamp")].toUtf8()).object();
    }

    json[QLatin1String("content")]     = QJsonDocument::fromJson(getContents()[QLatin1String("content")].toUtf8()).object();

    return json;
}

}
