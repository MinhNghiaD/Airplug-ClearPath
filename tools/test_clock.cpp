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
    qDebug() << "clock 1 after +1: " << QJsonDocument(clock1.convertToJson()).toJson();

    ++clock2;
    qDebug() << "clock 2 after +1: " << QJsonDocument(clock2.convertToJson()).toJson();

    clock1.updateClock(clock2);
    qDebug() << "clock 1 after receive from clock 2: " << QJsonDocument(clock1.convertToJson()).toJson();

    clock2.updateClock(clock1);
    qDebug() << "clock 2 after receive from clock 1: " << QJsonDocument(clock2.convertToJson()).toJson();

    qDebug() << "uuid : " << QUuid::createUuid().toByteArray();
}
