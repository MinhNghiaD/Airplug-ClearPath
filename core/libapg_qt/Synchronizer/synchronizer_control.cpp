#include "synchronizer_control.h"
namespace AirPlug
{
class SynchronizerControl::Private
{
public:

    explicit Q_DECL_HIDDEN Private(const QString& siteID)
        : siteID(siteID),
          activated(false),
          nbApps(0)
    {
    }

    ~Private()
    {
    }

public:

    QString siteID;
    // id of Base application who is the initaitor
    QString initiator;
    // id of NET application who in charge of initiator
    QString initiatorSite;
    bool    activated;
    bool    isInitiator;
    int     nbApps;
    int     nbWaitMsg;
    int     nbWaitAck;
};

SynchronizerControl::SynchronizerControl(const QString& siteID)
    : QObject(),
      d(new Private(siteID))
{
}

SynchronizerControl::~SynchronizerControl()
{
    delete d;
}

void SynchronizerControl::callElection(const QString& baseID)
{
    // save temporary this information before enter an election
    d->initiator = baseID;
    d->activated = true;
    emit signalRequestElection();
}

// NOTE init can only be called when the host win the election
void SynchronizerControl::init(const QString& initiatorSite)
{
    // set initiator, nbWaitMsg and nbWaitAck
    // NOTE: nbWaitMsg is equal to the nbApps (nb total of Base applications) in the network, including itself
    //       nbWaitAck is equal to nbApps - 1 (not including initiator)

    d->initiatorSite = initiatorSite;

    d->nbWaitMsg = d->nbApps;
    d->nbWaitAck = d->nbApps - 1;

    d->isInitiator = true;

    // NOTE : after initiated at Control level, it will give a permission to the base application
    // who will be the initiator to send the first message of the next cycle of synchronous process
    ACLMessage permission(ACLMessage::SYNC_ACK);

    QJsonObject content;
    content[QLatin1String("initiator")] = d->initiator;

    permission.setContent(content);

    emit signalSendToApp(permission);

}

// preprocess messages come from local base application before handing it to the rest of the network
bool SynchronizerControl::processLocalMessage(ACLMessage& message)
{
    // process only SYNC message
    if (message.getPerformative() == ACLMessage::SYNC)
    {
        QJsonObject content = message.getContent();

        // get siteID of the base application who send this message
        QString baseid = content[QLatin1String("siteID")].toString();

        // Election can only be called by the first base message arrives
        if (! d->activated)
        {
            // if the synchronization is not yet activated, call an election and candidate for initiator
            callElection(baseid);

            // This message will not be continued to passed to the network
            return false;
        }
        else
        {
            if (d->isInitiator)
            {
                QJsonObject content = message.getContent();
                content[QLatin1String("initiator")] = d->initiator;

                message.setContent(content);
            }

            if (--(d->nbWaitMsg) == 0)
            {
                // send ack to initiator
                ACLMessage ack(ACLMessage::SYNC_ACK);

                QJsonObject content;
                content[QLatin1String("initiator")] = d->initiator;

                ack.setContent(content);

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


void SynchronizerControl::setNbOfApp(int nbApps)
{
    d->nbApps = nbApps;
}


}
