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
    QString initiator;
    bool    activated;
    bool    isInitiator;
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
    // TODO SYNCHRONIZER 3 : set initiator, nbWaitMsg and nbWaitAck
    // NOTE: nbWaitMsg is equal to the nbApps (nb total of Base applications) in the network, including itself
    //       nbWaitAck is equal to nbApps - 1 (not including initiator)

    d->initiator = initiator;
    d->nbWaitMsg = d->nbApps;
    d->nbWaitAck = d->nbApps - 1;


    // NOTE : after initiated at Control level, it will give a permission to the base application who will be the initiator
    // to send the first message of the next cycle of synchronous process

    d->activated = true;
    d->isInitiator = true;

    ACLMessage permission(ACLMessage::SYNC_ACK);

    QJsonObject content;
    content[QLatin1String("initiator")] = initiator;

    permission.setContent(content);

    emit signalSendToApp(permission);
}

void SynchronizerControl::setNbOfApp(int nbApps)
{
    d->nbApps = nbApps;
}

bool SynchronizerControl::processLocalMessage(ACLMessage& message)
{
    // process only SYNC message
    if (message.getPerformative() == ACLMessage::SYNC)
    {
        QJsonObject content = message.getContent();

        // TODO SYNCHRONIZER 1 : get siteID of the base application who send this message
        QString sender_id = message.getSender();

        if (! d->activated)
        {
            // TODO SYNCHRONIZER 2 : if the synchronization is not yet activated, call init() to activate
            init(sender_id);

            return false;
        }
        else
        {
            // NOTE: here, after the local message is controlled, it is sent out to the network by a signal,
            // this signal will be connected to a slot in Router to send the message
            if (d->isInitiator)
            {
                QJsonObject content = message.getContent();
                content[QLatin1String("initiator")] = d->initiator;

                message.setContent(content);
                emit signalSendToNet(message);

            }

            if (--(d->nbWaitMsg) == 0)
            {
                // send ack to initiator
                ACLMessage ack(ACLMessage::SYNC_ACK);
                ack.setReceiver(d->initiator);
                emit signalSendToApp(ack);
            }
        }
    }

    return true;
}

void SynchronizerControl::processExternalMessage(ACLMessage& message)
{
    switch (message.getPerformative())
    {
        case ACLMessage::SYNC:
            // TODO SYNCHRONIZER 5 : process SYNC messages come from network
            // If SYNC message come from initiator, send a SYN_ACK message to local application to start the next cycle
            // i.e: check the field "initiator" in the content of the message
            // Decrement nbWaitMsg, then pass the message to application
            // if nbWaitMsg reachs 0 -> send a SYNC_ACK message back to initiator by emit signal signalSendToNet

            if (message.getContent()["initiator"] == message.getSender())
            {
                d->nbWaitMsg--;
                emit signalSendToApp(message);
                if (d->nbWaitMsg == 0)
                {
                    ACLMessage ack (ACLMessage::SYNC_ACK);
                    ack.setReceiver(d->initiator);
                    emit signalSendToNet(ack);
                }
            }

            break;
        case ACLMessage::SYNC_ACK:
            if (d->isInitiator)
            {
                // TODO SYNCHRONIZER 6 : decrement nbwaitAck, if nbwaitAck reaches 0,
                // pass SYNC_ACK message to base initiator application to start the next cycle

                d->nbWaitAck--;
                if (d->nbWaitAck == 0)
                {
                    emit signalSendToApp(message);
                }
            }

            break;
        default:
            break;
    }
}

}
