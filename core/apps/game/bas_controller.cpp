#include "bas_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>


using namespace AirPlug;

namespace BasApplication
{

class Q_DECL_HIDDEN BasController::Private
{
public:

    Private()
        : timer(nullptr),
          nbSequence(0)
    {
        // TODO: get default message ffrom QSettings
        messageToSend = QLatin1String("~");
    }

    ~Private()
    {
        delete timer;
    }

public:

    QTimer*               timer;

    QString               messageToSend;
    int                   nbSequence;
};



BasController::BasController(QObject* parent)
    : ApplicationController(QLatin1String("BAS"), parent),
      d(new Private)
{
}

BasController::~BasController()
{
    delete d;
}

void BasController::init(const QCoreApplication& app)
{
    ApplicationController::init(app);


    if (m_optionParser.autoSend && m_optionParser.delay > 0)
    {
        slotActivateTimer(m_optionParser.delay);
    }
}

void BasController::pause(bool b)
{
    m_optionParser.start = !b;

    if (b)
    {
        slotDeactivateTimer();
    }
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

        connect(d->timer, &QTimer::timeout,
                    this, &BasController::slotSendMessage);
    }

    m_optionParser.delay    = period;
    m_optionParser.autoSend = true;

    d->timer->start(period);
}

void BasController::slotDeactivateTimer()
{
    m_optionParser.autoSend = false;
    m_optionParser.delay    = 0;

    if (d->timer)
    {
        d->timer->stop();
    }
}

void BasController::slotPeriodChanged(int period)
{
    m_optionParser.delay = period;

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

    // TODO: get what, where, who from user interface
    sendMessage(message, QString(), QString(), QString());

    ++d->nbSequence;

    emit signalSequenceChange(d->nbSequence);
}

void BasController::slotReceiveMessage(Header header, Message message)
{
    emit signalMessageReceived(header, message);
}

}
