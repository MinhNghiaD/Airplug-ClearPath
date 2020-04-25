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

    Message* message2 = new Message(message.getMessage());

    ACLMessage* aclMessage2 = static_cast<ACLMessage*>(message2);

    qDebug() << "message 2:" << aclMessage2->getMessage();
}
