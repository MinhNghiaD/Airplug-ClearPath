#include "router.h"

// Qt includes
#include <QDebug>
#include <QThread>
#include <QPair>

namespace AirPlug
{

class Q_DECL_HIDDEN Router::Private
{
public:
    Private()
        : communicationMngr(nullptr),
          nbSequence(0),
          snapshot(nullptr)
    {
    }

    ~Private()
    {
    }

public:

    void forwardAppToNet(Header& header, ACLMessage& message);
    void forwardNetToApp(Header& header, ACLMessage& message);
    void forwardStateMessage(ACLMessage& message, bool fromLocal);
    void forwardPrepost(ACLMessage message, bool newPrepost);

public:

    bool isOldMessage(const ACLMessage& messsage);

public:

    CommunicationManager* communicationMngr;

    // using siteID and sequence nb to identify old message
    QString               siteID;
    int                   nbSequence;

    // map external router with : - first, the most nbSequence received  - second, the nb of active applications hosted at each site
    QHash<QString, QPair<int, int> > neighborInfo;

    LaiYangSnapshot*      snapshot;
};


void Router::Private::forwardAppToNet(Header& header, ACLMessage& message)
{
    // mark message ID
    message.setSender(siteID);
    message.setNbSequence(++nbSequence);

    QJsonObject contents =  message.getContent();

    // set App name
    contents[QLatin1String("app")] = header.what();

    if (snapshot)
    {
        snapshot->colorMessage(contents);
    }

    message.setContent(contents);

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}

void Router::Private::forwardNetToApp(Header& header, ACLMessage& message)
{
    if (! message.getContents().contains(QLatin1String("receiver")))
    {
        // broadcast the message to another net
        communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), header.where());
    }
    else
    {
        // TODO routing
    }

    QJsonObject contents =  message.getContent();

    QString app = contents[QLatin1String("app")].toString();
    contents.remove(QLatin1String("app"));

    if (snapshot)
    {
        QThread::msleep(1);
        if (snapshot->getColor(contents))
        {
            // detected prepost message
            forwardPrepost(message, true);
        }
    }

    message.setContent(contents);

    // NOTE: avoid signal lost at receiver
    QThread::msleep(1);

    communicationMngr->send(message, QLatin1String("NET"), app, Header::localHost);
}

void Router::Private::forwardStateMessage(ACLMessage& message, bool fromLocal)
{
    if (snapshot && (snapshot->processStateMessage(message, fromLocal) == false))
    {
        return;
    }

    // Forward to Network
    // mark message ID if state come from local app
    if (fromLocal)
    {
        message.setSender(siteID);
        message.setNbSequence(++nbSequence);
    }

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}

void Router::Private::forwardPrepost(ACLMessage message, bool newPrepost)
{
    if (newPrepost)
    {
        QJsonObject originalMessage;
        originalMessage[QLatin1String("receiver")] = siteID;
        originalMessage[QLatin1String("message")]  = message.getMessage();

        // encapsulate original message into Prepost message content
        message.setContent(originalMessage);

        message.setPerformative(ACLMessage::PREPOST_MESSAGE);
        message.setSender(siteID);
        message.setNbSequence(++nbSequence);
    }

    if (snapshot && snapshot->processPrePostMessage(message))
    {
        communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
    }
}

bool Router::Private::isOldMessage(const ACLMessage& messsage)
{
    QString sender = messsage.getSender();
    int sequence   = messsage.getNbSequence();

    if (sender == siteID)
    {
        //qDebug() << siteID << "Drop round back message";

        return true;
    }

    if ( neighborInfo.contains(sender) && (neighborInfo[sender].first >= sequence) )
    {
        // Here we suppose the channels are FIFO
        // therefore for each router, by keeing the sequence number of each site, we can identify old repeated message
        //qDebug() << siteID << ": Drop old message because recent sequence = " << neighborInfo[sender] << " > this sequence = " << sequence;

        return true;
    }

    // Update recent sequence
    neighborInfo[sender].first = sequence;

    return false;
}


/*----------------------------------------------------------------------------------------------------------------------------------------------------*/


Router::Router(CommunicationManager* communication, const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    if (!communication)
    {
        qFatal("Router: Communication Manager is null");
    }

    d->communicationMngr = communication;
    d->siteID            = siteID;

    setObjectName(QLatin1String("Router"));

    connect(d->communicationMngr, SIGNAL(signalMessageReceived(Header, Message)),
            this,                 SLOT(slotReceiveMessage(Header, Message)));
}

Router::~Router()
{
    delete d;
}

bool Router::addSnapshot(LaiYangSnapshot* snapshot)
{
    if (!snapshot)
    {
        return false;
    }

    d->snapshot = snapshot;

    connect(d->snapshot, &LaiYangSnapshot::signalRequestSnapshot,
            this,        &Router::slotSendMarker, Qt::DirectConnection);


    return true;
}

void Router::slotReceiveMessage(Header header, Message message)
{
    // cast message to ACL format
    ACLMessage aclMessage(*(static_cast<ACLMessage*>(&message)));

    if (header.what() == QLatin1String("NET"))
    {
        if (d->isOldMessage(aclMessage))
        {
            return;
        }

        switch (aclMessage.getPerformative())
        {
            case ACLMessage::INFORM_STATE:
                d->forwardStateMessage(aclMessage, false);

                break;
            case ACLMessage::PREPOST_MESSAGE:
                d->forwardPrepost(aclMessage, false);

                break;
            default:
                d->forwardNetToApp(header, aclMessage);

                break;
        }
    }
    else
    {
        if (aclMessage.getPerformative() == ACLMessage::INFORM_STATE)
        {
            // process state message
            d->forwardStateMessage(aclMessage, true);
        }
        else
        {
            d->forwardAppToNet(header, aclMessage);
        }
    }
}

void Router::slotSendMarker(const Message* marker)
{
    // broadcast to all app in local site
    d->communicationMngr->send(*marker, QLatin1String("NET"), Header::allApp, Header::localHost);
}

void Router::slotHeathCheck()
{
    ACLMessage ping(ACLMessage::PING);

    d->communicationMngr->send(ping, QLatin1String("NET"), Header::allApp, Header::localHost);
}

}
