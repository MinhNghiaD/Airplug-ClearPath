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
    bool activated;
    QString siteID;
    QString initiator;
    QString initiatorSite;
    int nbApps;
    int nbWaitMsg;
    int nbWaitAck;
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

void SynchronizerControl::processLocalMessage(ACLMessage& message)
{
    ACLMessage::Performative performative = message.getPerformative();

    if (performative == ACLMessage::SYNC)
    {
        QJsonObject content = message.getContent();

        if (content[QLatin1String("init")] == true)
        {
            callElection(content[QLatin1String("siteID")].toString());

            return;
        }

        --(d->nbWaitMsg);

        // TODO broadcast to local and to net
    }
    else if (performative == ACLMessage::SYNC_ACK)
    {
        if (d->initiatorSite == d->siteID)
        {
            // Initiator is in the same site
            --(d->nbWaitAck);

            if (d->nbWaitAck == 0)
            {
                ACLMessage permission(ACLMessage::SYNC_ACK);

                QJsonObject content;
                content[QLatin1String("forInitiator")] = true;

                permission.setContent(content);

                // give permission to initiator
                emit signalSendToApp(permission);
            }
        }
        else
        {
            // Foward to initiator site
            message.setReceiver(d->initiatorSite);

            emit signalSendToNet(message);
        }
    }
}

void SynchronizerControl::processExternalMessage(ACLMessage& message)
{
    ACLMessage::Performative performative = message.getPerformative();

    QJsonObject content = message.getContent();

    if (performative == ACLMessage::SYNC)
    {
        if (!d->activated)
        {
            if (content[QLatin1String("fromInitiator")].toBool())
            {
                d->initiatorSite = message.getSender();
                d->initiator     = message.getTimeStamp()->getSiteID();
                d->activated     = true;
            }
        }

        // TODO : forward SYNC to app

        --(d->nbWaitMsg);

        if (d->nbWaitMsg)
        {
            ACLMessage permission(ACLMessage::SYNC_ACK);

            // give the app permission to perform next step
            emit signalSendToApp(permission);
        }
    }
    else if (performative == ACLMessage::SYNC_ACK)
    {
        if(d->siteID == message.getReceiver())
        {
            --(d->nbWaitAck);

            if (d->nbWaitAck == 0)
            {
                ACLMessage permission(ACLMessage::SYNC_ACK);

                QJsonObject content;
                content[QLatin1String("forInitiator")] = true;

                permission.setContent(content);

                // give permission to initiator
                emit signalSendToApp(permission);
            }
        }
    }
}

void SynchronizerControl::setNbOfApp(int nbApps)
{
    d->nbApps = nbApps;
}


void SynchronizerControl::callElection(const QString& baseID)
{
    if (!d->activated)
    {
        // save temporary this information before enter an election
        d->initiator = baseID;
        d->activated = true;

        emit signalRequestElection();
    }
}

void SynchronizerControl::init(const QString& initiatorSite)
{
    // This method is called when local site wins the election

    //d->initiatorSite = initiatorSite;
    //d->isInitiator = true;
    d->nbWaitMsg = d->nbApps;
    d->nbWaitAck = d->nbApps - 1;

    // NOTE : after initiated at Control level, it will give a permission to the base application
    // who will be the initiator to send the first message of the next cycle of synchronous process
    ACLMessage permission(ACLMessage::SYNC_ACK);

    QJsonObject content;
    content[QLatin1String("forInitiator")] = true;
    content[QLatin1String("initiator")]    = d->initiator;

    permission.setContent(content);

    emit signalSendToApp(permission);
}

// TODO : Handle finish election (after activated or at the end of sync)
}
