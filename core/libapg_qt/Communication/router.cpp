#include "router.h"

// Qt includes
#include <QDebug>
#include <QPair>
#include <QTimer>

namespace AirPlug
{

class Q_DECL_HIDDEN Router::Private
{
public:
    Private()
        : communicationMngr(nullptr),
          nbSequence(0),
          nbApp(0),
          temporaryNbApp(0),
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
    void forwardPrepost(const ACLMessage& message);
    void forwardRecover(const ACLMessage& message);
    void forwardReady(const ACLMessage& message);

    bool isOldMessage(const ACLMessage& messsage);

    void updateActiveNeighbor(const ACLMessage& messsage);
    int  nbOfApp() const;
    int  nbOfNeighbor() const;

public:

    CommunicationManager* communicationMngr;

    // using siteID and sequence nb to identify old message
    QString               siteID;
    int                   nbSequence;
    int                   nbApp;
    int                   temporaryNbApp;

    LaiYangSnapshot*      snapshot;

    // map external router with : - first, the most nbSequence received  - second, the nb of active applications hosted at each site
    QHash<QString, QPair<int, int> > neighborInfo;
    QVector<QString>                 activeNeighBors;
};


void Router::Private::forwardAppToNet(Header& header, ACLMessage& message)
{
    // mark message ID
    message.setSender(siteID);
    message.setNbSequence(++nbSequence);

    // TODO: mark receiver for routing

    QJsonObject contents = message.getContent();

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

    QJsonObject contents = message.getContent();

    QString app = contents[QLatin1String("app")].toString();
    contents.remove(QLatin1String("app"));

    if (snapshot)
    {
        if (snapshot->getColor(contents))
        {
            // detected prepost message
            ACLMessage prepost = snapshot->encodePrepostMessage(message);

            prepost.setSender(siteID);
            prepost.setNbSequence(++nbSequence);

            forwardPrepost(prepost);
        }
    }

    message.setContent(contents);

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

void Router::Private::forwardPrepost(const ACLMessage& prepost)
{
    if (snapshot && !snapshot->processPrePostMessage(prepost))
    {
        return;
    }

    communicationMngr->send(prepost, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}

void Router::Private::forwardRecover(const ACLMessage& message)
{
    if (snapshot && !snapshot->processRecoveringMessage(message))
    {
        return;
    }

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}

void Router::Private::forwardReady(const ACLMessage& message)
{
    if (snapshot && !snapshot->processReadyMessage(message))
    {
        return;
    }

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
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

        return true;
    }

    // Update recent sequence
    neighborInfo[sender].first = sequence;

    return false;
}

void Router::Private::updateActiveNeighbor(const ACLMessage& messsage)
{
    neighborInfo[messsage.getSender()].second = (messsage.getContent()[QLatin1String("nbApp")].toInt());

    if (snapshot)
    {
        snapshot->setNbOfApp(nbOfApp());
        snapshot->setNbOfNeighbor(nbOfNeighbor());
    }

    // forward
    communicationMngr->send(messsage, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}


int Router::Private::nbOfApp() const
{
    int totalNbApp = nbApp;

    for (QHash<QString, QPair<int, int> >::const_iterator iter  = neighborInfo.cbegin();
                                                          iter != neighborInfo.cend();
                                                          ++iter)
    {
        totalNbApp += iter.value().second;
    }

    return totalNbApp;
}

int Router::Private::nbOfNeighbor() const
{
    int nbNeighbor = 0;

    for (QHash<QString, QPair<int, int> >::const_iterator iter  = neighborInfo.cbegin();
                                                          iter != neighborInfo.cend();
                                                          ++iter)
    {
        if (iter.value().second > 0)
        {
            ++nbNeighbor;
        }
    }

    return nbNeighbor;
}

/*------------------------------------------------------------ Router implementations ----------------------------------------------------------------------------------------*/


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

    // health check neighbors periodically
    QTimer::singleShot(30000, this, SLOT(slotRefreshActiveNeighbor()));
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

    connect(d->snapshot, &LaiYangSnapshot::signalSendSnapshotMessage,
            this,        &Router::slotForwardSnapshotMessage, Qt::DirectConnection);

    return true;
}


// Main event handler
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

        // active neighbor : TODO  check this
        QString neighborID = aclMessage.getSender();

        if(! d->activeNeighBors.contains(neighborID))
        {
            d->activeNeighBors.append(neighborID);
        }

        switch (aclMessage.getPerformative())
        {
            case ACLMessage::INFORM_STATE:
                d->forwardStateMessage(aclMessage, false);
                break;

            case ACLMessage::PREPOST_MESSAGE:
                d->forwardPrepost(aclMessage);
                break;

            case ACLMessage::SNAPSHOT_RECOVER:
                d->forwardRecover(aclMessage);
                break;

            case ACLMessage::READY_SNAPSHOT:
                d->forwardReady(aclMessage);
                break;

            case ACLMessage::UPDATE_ACTIVE:
                d->updateActiveNeighbor(aclMessage);
                break;

            default:
                d->forwardNetToApp(header, aclMessage);
                break;
        }
    }
    else
    {
        switch (aclMessage.getPerformative())
        {
            case ACLMessage::INFORM_STATE:
                // receive local state
                d->forwardStateMessage(aclMessage, true);
                break;

            case ACLMessage::PONG:
                ++(d->temporaryNbApp);
                break;

            default:
                d->forwardAppToNet(header, aclMessage);
                break;
        }
    }
}

void Router::slotSendMarker(const Message& marker)
{
    // broadcast to all app in local site
    d->communicationMngr->send(marker, QLatin1String("NET"), Header::allApp, Header::localHost);
}

void Router::slotForwardSnapshotMessage(ACLMessage& message)
{
    // broadcast to all app in local site
    message.setSender(d->siteID);
    message.setNbSequence(++d->nbSequence);

    d->communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}


void Router::slotHeathCheck()
{
    d->temporaryNbApp = 0;

    ACLMessage ping(ACLMessage::PING);

    d->communicationMngr->send(ping, QLatin1String("NET"), Header::allApp, Header::localHost);

    // activate timeout timer of 4s
    QTimer::singleShot(4000, this, SLOT(slotPingTimeOut()));

    // recheck after 20s
    QTimer::singleShot(20000, this, SLOT(slotHeathCheck()));
}

void Router::slotPingTimeOut()
{
    // update nb of active application
    d->nbApp = d->temporaryNbApp;

    if (d->snapshot)
    {
        d->snapshot->setNbOfApp(d->nbOfApp());
        d->snapshot->setNbOfNeighbor(d->nbOfNeighbor());
    }

    // broadcast local nb of app to other sites
    // mark message ID
    ACLMessage message(ACLMessage::UPDATE_ACTIVE);

    message.setSender(d->siteID);
    message.setNbSequence(++d->nbSequence);

    QJsonObject contents;

    contents[QLatin1String("nbApp")] = d->nbApp;
    message.setContent(contents);

    d->communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}


void Router::slotRefreshActiveNeighbor()
{
    QStringList allNeighBor = d->neighborInfo.keys();

    for (QStringList::const_iterator iter  = allNeighBor.cbegin();
                                     iter != allNeighBor.cend();
                                     ++iter)
    {
        // every neighbor that doesn't have any activity withit 30s is considered as inactive
        if (! d->activeNeighBors.contains(*iter))
        {
            d->neighborInfo[*iter].second = 0;
        }
    }

    d->activeNeighBors.clear();

    // reactivate timer
    QTimer::singleShot(30000, this, SLOT(slotRefreshActiveNeighbor()));
}

}
