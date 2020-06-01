#include "synchronizer_base.h"
namespace AirPlug
{
class SynchronizerBase::Private
{
public:

    explicit Q_DECL_HIDDEN Private(const QString& siteID)
        : siteID(siteID)
    {
    }

    ~Private()
    {
    }

public:

    QString siteID;
};

SynchronizerBase::SynchronizerBase(const QString& siteID)
    : QObject(),
      d(new Private(siteID))
{
}

SynchronizerBase::~SynchronizerBase()
{
    delete d;
}

void SynchronizerBase::init()
{
    ACLMessage message(ACLMessage::SYNC);
    // set the name of base application in the content of the message and send a dummy message to NET to candidate
    QJsonObject content;
    content[QLatin1String("siteID")] = d->siteID;
    message.setContent(content);
    message.setSender(d->siteID);

    emit signalSendMessage(message);
}

void SynchronizerBase::processMessage(ACLMessage& message)
{
    // Process message from NET,
    // If the message is a SYNC_ACK message,
    // prepare an ACLMessage of performative SYNC with the siteID as the first content, as an envelop,
    // then emit signalDoStep() to inform application to start the next cycle.
    // The application will use this envelop to send the message in the next cycle

    // If not SYNC_ACK, do nothing

    if (message.getPerformative() != ACLMessage::SYNC_ACK)
    {
        return;
    }

    QJsonObject content;
    ACLMessage answer (ACLMessage::SYNC);
    content[QLatin1String("siteID")] = d->siteID;
    answer.setContent(content);

    emit signalDoStep(answer);
}


}
