// Qt includes
#include <QCoreApplication>

#include <QString>
#include <QDebug>

// Local includes
#include "aclmessage.h"
#include "header.h"
#include "vector_clock.h"

using namespace AirPlug;

int main(int argc, char *argv[])
{
    ACLMessage message(ACLMessage::INFORM);

     VectorClock clock1(QLatin1String("Site1"));
     ++clock1;

    message.setTimeStamp(clock1);

    QJsonObject content;
    content[QLatin1String("color")] = QLatin1String("white");
    content[QLatin1String("message")] = QLatin1String("haha");

    message.setContent(content);

    qDebug() << "message 1:" << message.getMessage();

    Message casted(message);

    qDebug() << "casted message" << casted.getMessage();

    ACLMessage aclMessage2(*(static_cast<ACLMessage*>(&casted)));

    VectorClock* timestamp = aclMessage2.getTimeStamp();

    if (timestamp)
    {
        qDebug() << "timestamp :" << QJsonDocument(timestamp->convertToJson()).toJson(QJsonDocument::Compact);
    }

    content = aclMessage2.getContent();

    content.remove(QLatin1String("color"));

    aclMessage2.setContent(content);

    qDebug() << "message 2" << aclMessage2.getMessage();

    qDebug() << "content :" <<  QJsonDocument(aclMessage2.getContent()).toJson(QJsonDocument::Compact);

    qDebug() << "performative :" <<  aclMessage2.getPerformative();

    ACLMessage& message2Reference = aclMessage2;

    ACLMessage message3 = message2Reference;

    QJsonObject newContent;
    newContent["value"] = 5;

    message2Reference.setContent(newContent);
    qDebug() << "reference message 2" << message2Reference.getMessage();
    qDebug() << "message 3 : " << message3.getMessage();

    Message message4(message3);
    qDebug() << "message 4 : " << message4.getMessage();

    qDebug() << "json message 3" << QJsonDocument(message3.toJsonObject()).toJson(QJsonDocument::Compact);
}
