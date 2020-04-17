#include "application_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>

namespace AirPlug
{

class ApplicationController::Private
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



ApplicationController::ApplicationController(QObject* parent)
    : QObject(parent),
       d(new Private)
{
    setObjectName(QLatin1String("BAS"));
}

ApplicationController::~ApplicationController()
{
    delete d;
}

void ApplicationController::parseOptions(const QCoreApplication& app)
{
    d->optionParser = new OptionParser(app);

    // Debug
    d->optionParser->showOption();

    d->communication = new CommunicationManager(d->optionParser->ident,
                                                d->optionParser->destination,
                                                Header::airHost,
                                                d->optionParser->headerMode,
                                                this);

    d->communication->setSafeMode(d->optionParser->safemode);

    d->communication->subscribeAir(d->optionParser->source);

    connect(d->communication, &CommunicationManager::signalMessageReceived,
            this,             &ApplicationController::signalMessageReceived);


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


    if (d->optionParser->autoSend && d->optionParser->delay > 0)
    {
        slotActivateTimer(d->optionParser->delay);
    }
}

int ApplicationController::getPeriod()  const
{
    return d->optionParser->delay;
}

bool ApplicationController::hasGUI() const
{
    return (!d->optionParser->nogui);
}

bool ApplicationController::isStarted() const
{
    return (d->optionParser->start);
}

void ApplicationController::pause(bool b)
{
    d->optionParser->start = !b;

    if (b)
    {
        slotDeactivateTimer();
    }
}

bool ApplicationController::isAuto() const
{
    return (d->optionParser->autoSend);
}

void ApplicationController::setMessage(const QString& msg)
{
    d->messageToSend = msg;
}

Header::HeaderMode ApplicationController::headerMode() const
{
    return d->optionParser->headerMode;
}

void ApplicationController::slotActivateTimer(int period)
{
    if (! d->timer)
    {
        d->timer = new QTimer(this);

        connect(d->timer, &QTimer::timeout,
                    this, &ApplicationController::slotSendMessage);
    }

    d->optionParser->delay    = period;
    d->optionParser->autoSend = true;

    d->timer->start(period);
}

void ApplicationController::slotDeactivateTimer()
{
    d->optionParser->autoSend = false;
    d->optionParser->delay    = 0;

    if (d->timer)
    {
        d->timer->stop();
    }
}

void ApplicationController::slotPeriodChanged(int period)
{
    d->optionParser->delay = period;

    if (d->timer)
    {
        d->timer->setInterval(period);
    }
}

void ApplicationController::slotSendMessage()
{
    Message message;

    message.addContent(QLatin1String("payload"), d->messageToSend);
    message.addContent(QLatin1String("nseq"), QString::number(d->nbSequence));

    // TODO: get what, where, who from user interface
    if(d->optionParser->remote)
    {
        d->communication->send(message, QString(), QString(), QString(),
                               CommunicationManager::ProtocolType::UDP,
                               d->optionParser->save);
    }
    else
    {
        d->communication->send(message, QString(), QString(), QString(),
                               CommunicationManager::ProtocolType::StandardIO,
                               d->optionParser->save);
    }

    ++d->nbSequence;

    emit signalSequenceChange(d->nbSequence);
}

}

