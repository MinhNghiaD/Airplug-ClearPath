#include "synchronizer_base.h"
namespace AirPlug
{
class SynchronizerBase::Private
{
public:

    explicit Private(const QString& siteID)
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
    // TODO SYNCHRONIZER 0 : set the name of base application in the content of the message and send a dummy message to NET to candidate
    // The content of a message is encoded in a QJsonObject and can be set by using ACLMessage::setContent(const QJsonObject& content)
    // NOTE: siteID used in the system is of type QString
    /* Ex:
    QJsonObject content;
    content[QLatin1String("siteID")] = d->siteID;
    message.setContent(content);

    emit signalSendMessage(message);
    */
}

void SynchronizerBase::processMessage(ACLMessage& message)
{
    // TODO SYNCHRONIZER 4 : process message from NET,
    // If the message is a SYNC_ACK message,
    // prepare an ACLMessage of performative SYNC with the siteID as the first content, as an envelop,
    // then emit signalDoStep() to inform application to start the next cycle.
    // The application will use this envelop to send the message in the next cycle

    // If not SYNC_ACK, do nothing
}


}
