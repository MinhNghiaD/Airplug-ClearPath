#include "synchronizer_control.h"
namespace AirPlug
{
class SynchronizerControl::Private
{
public:

    explicit Private()
        : activated(false),
          nbApps(0)
    {
    }

    ~Private()
    {
    }

public:

    // TODO sync on Base application level
    QString initator;
    bool    activated;
    int     nbApps;
    int     nbWaitMsg;
    int     nbWaitAck;
};

SynchronizerControl::SynchronizerControl()
    : QObject(),
      d(new Private())
{
}

SynchronizerControl::~SynchronizerControl()
{
    delete d;
}

void SynchronizerControl::init(const QString& initiator)
{
    // TODO SYNCHRONIZER 3 : set initiator and, nbWaitMsg and nbWaitAck
    // NOTE: nbWaitMsg is equal to the nbApps (nb total of Base applications) in the network, including itself
    //       nbWaitAck is equel to nbApps - 1 (not including initator)



    // NOTE : after initated at Control level, it will give a permission to the base aplication who will be the initiator
    // to send the first message of the next cycle of synchronous process

    d->activated = true;

    ACLMessage permission(ACLMessage::SYNC_ACK);

    QJsonObject content;
    content[QLatin1String("initator")] = initiator;

    permission.setContent(content);

    emit signalSendToApp(permission);
}

void SynchronizerControl::setNbApps(int nbApps)
{
    d->nbApps = nbApps;
}

void SynchronizerControl::processLocalMessage(ACLMessage& message)
{
    QJsonObject content = message.getContent();

    // TODO SYNCHRONIZER 1 : get siteID of the base application who send this message

    if (! d->activated)
    {
        // TODO SYNCHRONIZER 2 : if the synchronization is not yet activated, call init to activate
    }
    else
    {
        // NOTE: here, after the local message is controlled, it is sent out to the network by a signal,
        // this signal will be connected to a slot in Router to send the message
        emit signalSendToNet(message);
    }

    --(d->nbWaitMsg);
}

}
