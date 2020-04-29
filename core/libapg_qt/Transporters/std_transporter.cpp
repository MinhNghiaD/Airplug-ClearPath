#include "std_transporter.h"

//C++ include
#include <iostream>
#include <string>

//Qt include
#include <QSocketNotifier>
#include <QDebug>

namespace AirPlug
{

class Q_DECL_HIDDEN StdTransporter::Private
{
public:

    Private()
        : notifier(nullptr)
    {
    }

    ~Private()
    {
        delete notifier;
    }

public:

    QSocketNotifier* notifier;
};

StdTransporter::StdTransporter(QObject* parent)
    : MessageTransporter(parent),
      d(new Private())
{
    setObjectName(QLatin1String("StandardIO Protocol"));

    d->notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);

    connect(d->notifier, &QSocketNotifier::activated,
            this,        &StdTransporter::slotMessageArrive, Qt::DirectConnection);
}

StdTransporter::~StdTransporter()
{
    delete d;
}


void StdTransporter::send(const QString& message)
{
    std::cout << message.toStdString() << std::endl;
}

void StdTransporter::slotMessageArrive()
{
    std::string message;
    std::getline(std::cin, message);

    emit signalMessageReceived(QString::fromStdString(message));
}

}
