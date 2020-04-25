// Qt includes
#include <QString>
#include <QJsonDocument>
#include <QUuid>
#include <QDebug>

// Local includes
#include "vector_clock.h"

using namespace AirPlug;

int main()
{
    VectorClock clock1(QLatin1String("Site1"));
    VectorClock clock2(QLatin1String("Site2"));

    ++clock1;
    qDebug() << "clock 1 after +1: " << QJsonDocument(clock1.convertToJson()).toJson(QJsonDocument::Compact);

    ++clock2;
    qDebug() << "clock 2 after +1: " << QJsonDocument(clock2.convertToJson()).toJson(QJsonDocument::Compact);

    clock1.updateClock(clock2);
    qDebug() << "clock 1 after receive from clock 2: " << QJsonDocument(clock1.convertToJson()).toJson(QJsonDocument::Compact);

    VectorClock clock3(clock1.convertToJson());
    qDebug() << "clock 3 copied from clock 1:        " << QJsonDocument(clock3.convertToJson()).toJson(QJsonDocument::Compact);


    clock2.updateClock(clock1);
    qDebug() << "clock 2 after receive from clock 1: " << QJsonDocument(clock2.convertToJson()).toJson(QJsonDocument::Compact);

    qDebug() << "uuid : " << QUuid::createUuid().toByteArray();
}
