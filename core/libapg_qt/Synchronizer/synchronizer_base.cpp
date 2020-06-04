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
    QString initiator;
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
    // send a dummy message to NET to candidate for initiator position of synchronous network
    QJsonObject content;
    content[QLatin1String("siteID")] = d->siteID;
    message.setContent(content);

    emit signalSendMessage(message);
}

void SynchronizerBase::processACKMessage(ACLMessage& message)
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

    QJsonObject content = message.getContent();

    // Only the first ACK message call by synchronizer init has this field
    if (content.contains(QLatin1String("initiator")))
    {
        d->initiator = content[QLatin1String("initiator")].toString();

        if (d->initiator != d->siteID)
        {
            // Ensure that only initiator does the first step
            return;
        }
    }

    emit signalDoStep();
}

void SynchronizerBase::processSYNCMessage(ACLMessage& message)
{
    // TODO SYNCHRONIZER 11:
}

}
