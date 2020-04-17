#include "bas_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>


using namespace AirPlug;

namespace BasApplication
{

class BasController::Private
{
public:

    Private()
        : communication(nullptr),
          optionParser(nullptr),
          timer(nullptr),
          nbSequence(0)
    {
        // TODO: get default message ffrom QSettings
        messageToSend = QLatin1String("~");
    }

    ~Private()
    {
        delete communication;
        delete optionParser;
        delete timer;
    }

public:

    CommunicationManager* communication;

    OptionParser*         optionParser;

    QTimer*               timer;

    QString               messageToSend;
    int                   nbSequence;
};



BasController::BasController(QObject* parent)
    : QObject(parent),
       d(new Private)
{
    setObjectName(QLatin1String("BAS"));

    d->communication = new CommunicationManager(Header::HeaderMode::WhatWho, this);
    //d->communication->addStdTransporter();

    // TODO: get default messageToSend from QSettings
}

BasController::~BasController()
{
    delete d;
}

void BasController::parseOptions(const QCoreApplication& app)
{
    d->optionParser = new OptionParser(app);

    // Debug
    d->optionParser->showOption();

    if (d->optionParser->remote)
    {
        d->communication->addUdpTransporter(d->optionParser->remoteHost,
                                            d->optionParser->remotePort,
                                            MessageTransporter::MultiCast);
    }
    else
    {
        d->communication->addStdTransporter();
    }

    d->communication->setHeaderMode(d->optionParser->headerMode);
    d->communication->setSafeMode(d->optionParser->safemode);

    if (d->optionParser->autoSend && d->optionParser->delay > 0)
    {
        slotActivateTimer(d->optionParser->delay);
    }
}

int BasController::getPeriod()  const
{
    return d->optionParser->delay;
}

bool BasController::hasGUI() const
{
    return (!d->optionParser->nogui);
}

bool BasController::isStarted() const
{
    return (d->optionParser->start);
}

void BasController::pause(bool b)
{
    d->optionParser->start = !b;

    if (b)
    {
        slotDeactivateTimer();
    }
}

bool BasController::isAuto() const
{
    return (d->optionParser->autoSend);
}

void BasController::setMessage(const QString& msg)
{
    d->messageToSend = msg;
}

void BasController::slotActivateTimer(int period)
{
    if (! d->timer)
    {
        d->timer = new QTimer(this);

        connect(d->timer, SIGNAL(timeout()),
                    this, SLOT(slotSendMessage()));
    }

    d->optionParser->delay    = period;
    d->optionParser->autoSend = true;

    d->timer->start(period);
}

void BasController::slotDeactivateTimer()
{
    d->optionParser->autoSend = false;
    d->optionParser->delay    = 0;

    if (d->timer)
    {
        d->timer->stop();
    }
}

void BasController::slotPeriodChanged(int period)
{
    d->optionParser->delay = period;

    if (d->timer)
    {
        d->timer->setInterval(period);
    }
}

void BasController::slotSendMessage()
{
    Message message;

    message.addContent(QLatin1String("payload"), d->messageToSend);
    message.addContent(QLatin1String("nseq"), QString::number(d->nbSequence));


    if(d->optionParser->remote)
    {
        d->communication->send(message, QString(), QString(), QString(), CommunicationManager::ProtocolType::UDP, d->optionParser->save);
    }
    else
    {
        d->communication->send(message, QString(), QString(), QString(), CommunicationManager::ProtocolType::StandardIO, d->optionParser->save);
    }

    ++d->nbSequence;

    emit signalSequenceChange(d->nbSequence);
}
}
