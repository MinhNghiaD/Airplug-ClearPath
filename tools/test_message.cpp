// Qt includes
#include <QCoreApplication>

#include <QString>
#include <QDebug>

// Local includes
#include "message.h"
#include "header.h"

using namespace AirPlug;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("TEST");

    Message msg("init");

    msg.parseText(QLatin1String("^value~10^key~x"));

    qDebug() << "msg 1 " << msg.getMessage();

    Message msg2(msg.getMessage());

    msg2.parseText(msg.getMessage());


    return a.exec();
}
