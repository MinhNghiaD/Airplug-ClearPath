// Qt includes
#include <QCoreApplication>

#include <QString>
#include <QDebug>

// Local includes
#include "src/Frame/package.h"

using namespace AirPlug;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("TEST");

    Message msg("init");

    msg.parseText(QLatin1String("^value~10^key~x"));

    qDebug() << msg.getMessage();

    Message msg2("init");

    msg2.parseText(msg.getMessage());

    Header defaultHeader;

    qDebug() << "header:" << defaultHeader.generateHeader(Header::HeaderMode::WhatWhoWhere);


    return a.exec();
}
