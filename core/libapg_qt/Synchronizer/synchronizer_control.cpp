#include "synchronizer_control.h"

#include <QDebug>

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
    setObjectName(QLatin1String("Synchronizer Control"));
}

SynchronizerControl::~SynchronizerControl()
{
    if (d->initiatorSite == d->siteID)
    {
        // TODO : Handle finish election (after activated or at the end of sync)
        emit signalFinishElection(ElectionManager::Synchronizer);
    }

    delete d;
}

bool SynchronizerControl::processLocalMessage(ACLMessage& message)
{
    ACLMessage::Performative performative = message.getPerformative();

    if (performative == ACLMessage::SYNC)
    {
        QJsonObject content = message.getContent();

        if (content[QLatin1String("init")] == true)
        {
            callElection(content[QLatin1String("siteID")].toString());

            // stop to be routed
            return false;
        }

        --(d->nbWaitMsg);

        if (d->nbWaitMsg == 0)
        {
            ACLMessage permission(ACLMessage::SYNC_ACK);

            // give the app permission to perform next step
            emit signalSendToApp(permission);

            d->nbWaitMsg = d->nbApps;

            qDebug() << d->siteID << "collect all SYNC, give permission to local apps to do step";
        }
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

                d->nbWaitAck = d->nbApps - 1;

                qDebug() << d->siteID << "initiator receives all ACK --> unlock";
            }
        }
        else
        {
            // Foward to initiator site
            message.setReceiver(d->initiatorSite);

            emit signalSendToNet(message);
        }

        // take different routes
        return false;
    }

    // continue to be broadcast to local and to net
    return true;
}

bool SynchronizerControl::processExternalMessage(ACLMessage& message)
{
    ACLMessage::Performative performative = message.getPerformative();

    QJsonObject content = message.getContent();

    if (performative == ACLMessage::SYNC)
    {
        if (content[QLatin1String("fromInitiator")].toBool())
        {
            d->initiatorSite = message.getSender();
            d->initiator     = message.getTimeStamp()->getSiteID();
            d->nbWaitMsg     = d->nbApps;
            d->activated     = true;
        }

        --(d->nbWaitMsg);

        if (d->nbWaitMsg == 0)
        {
            ACLMessage permission(ACLMessage::SYNC_ACK);

            // give the app permission to perform next step
            emit signalSendToApp(permission);

            d->nbWaitMsg = d->nbApps;

            qDebug() << d->siteID << "collect all SYNC, give permission to local apps to do step";
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

                d->nbWaitAck = d->nbApps - 1;

                qDebug() << d->siteID << "initiator receives all ACK --> unlock";
            }
        }

        // Take another route
        return false;
    }

    // Continue to be sent to app
    return true;
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

        emit signalRequestElection(ElectionManager::Synchronizer);

        //qDebug() << d->initiator << "call election from" << d->siteID;
    }
}

void SynchronizerControl::init()
{
    // This method is called when local site wins the election
    //qDebug() << d->siteID << "wins election and" << d->initiator << "becomes initiator of synchronous network";

    d->initiatorSite = d->siteID;
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
}
