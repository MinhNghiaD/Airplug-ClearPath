#include "bas_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>

// Local includes
#include "communication_manager.h"

using namespace AirPlug;

namespace BasApplication
{

class BasController::Private
{
public:

    Private()
        : communication(nullptr),
          optionparser(nullptr),
          timer(nullptr),
          isAuto(false)
    {
    }

    ~Private()
    {
        delete communication;
        delete optionparser;
        delete timer;
    }

public:

    CommunicationManager* communication;

    OptionParser*         optionparser;

    QTimer*               timer;
    bool                  isAuto;

    QString               messageToSend;
};

BasController::BasController(QObject* parent)
    : QObject(parent),
       d(new Private)
{
    setObjectName(QLatin1String("BAS"));

    d->communication = new CommunicationManager(Header::HeaderMode::WhatWho, this);
    d->communication->addStdTransporter();

    // TODO: get default messageToSend from QSettings
}

BasController::~BasController()
{
    delete d;
}

void BasController::parseOptions(const QCoreApplication& app)
{
    d->optionparser = new OptionParser(app);

    d->optionparser->showOption();
}

void BasController::slotSendingMessageChanged(const QString& msg)
{
    d->messageToSend = msg;
}

void BasController::slotActivateTimer(int period)
{
    d->isAuto = true;

    if (! d->timer)
    {
        d->timer = new QTimer(this);

        connect(d->timer, SIGNAL(timeout()),
                    this, SLOT(slotSendMessage()));
    }

    d->timer->start(period);
}

void BasController::slotDeactivateTimer()
{
    d->isAuto = false;

    if (d->timer)
    {
        d->timer->stop();
    }
}

void BasController::slotPeriodChanged(int period)
{
    if (d->timer)
    {
        d->timer->setInterval(period);
    }
}

void BasController::slotSendMessage() const
{
    // TODO adapt to Header Message scheme
    qDebug() << d->messageToSend;
}

}
