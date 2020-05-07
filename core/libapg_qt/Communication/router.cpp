#include "router.h"

// Qt includes
#include <QDebug>
#include <QTimer>

// libapg include

#include "watchdog.h"

namespace AirPlug
{

class Q_DECL_HIDDEN Router::Private
{
public:
    Private()
        : communicationMngr(nullptr),
          nbSequence(0),
          nbApp(0),
          snapshot(nullptr),
          watchdog(nullptr)
    {
    }

    ~Private()
    {
        delete watchdog;
    }

public:

    void forwardAppToNet(Header& header, ACLMessage& message);
    void forwardNetToApp(Header& header, ACLMessage& message);

    void forwardStateMessage (ACLMessage& message, bool fromLocal);
    void forwardPrepost(const ACLMessage& message);
    void forwardRecover(const ACLMessage& message);
    void forwardReady  (const ACLMessage& message);
    void forwardPing   (const ACLMessage& message);
    void forwardPong   (const ACLMessage& message, bool fromLocal);

    void receiveMutexRequest (ACLMessage& request, bool fromLocal);
    void receiveMutexApproval(ACLMessage& request, bool fromLocal);
    void refuseAllPendingRequest();

    bool isOldMessage  (const ACLMessage& messsage);

public:

    CommunicationManager* communicationMngr;

    // using siteID and sequence nb to identify old message
    QString               siteID;
    int                   nbSequence;
    int                   nbApp;

    LaiYangSnapshot*      snapshot;
    Watchdog*             watchdog;

    // map external router with : - the most recent nbSequence received
    QHash<QString, int>   recentSequences;

    // each NET will keep track of its applications request
    QHash<QString, int>   localMutexWaitingList;
};


void Router::Private::forwardAppToNet(Header& header, ACLMessage& message)
{
    // Broadcast to all other applications in same site first
    communicationMngr->send(message, QLatin1String("NET"), Header::allApp, Header::localHost);

    // mark message ID
    message.setSender(siteID);
    message.setNbSequence(++nbSequence);

    // TODO: mark receiver for routing
    QJsonObject contents           = message.getContent();
    contents[QLatin1String("app")] = header.what();

    if (snapshot)
    {
        snapshot->colorMessage(contents);
    }

    message.setContent(contents);

    // forward to network
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
    QString     app      = contents[QLatin1String("app")].toString();

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

void Router::Private::forwardPing(const ACLMessage& message)
{
    watchdog->broadcastInfo();

    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}

void Router::Private::forwardPong(const ACLMessage& message, bool fromLocal)
{
    if (fromLocal)
    {
        bool        isNewApp = false;
        QJsonObject content  = message.getContent();

        if (content.contains(QLatin1String("isNew")))
        {
            isNewApp = content[QLatin1String("isNew")].toBool();
        }

        watchdog->receivePong(false);

        return;
    }

    // Receive pong from network => update network info for watchdog
    watchdog->receiveNetworkInfo(message);

    // forward
    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}


void Router::Private::receiveMutexRequest(ACLMessage& request, bool fromLocal)
{
    if (fromLocal)
    {
        // append to Waiting list
        VectorClock* timestamp = request.getTimeStamp();

        if (!timestamp)
        {
            qWarning() << "receiveMutexRequest: local clock is null";
            return;
        }

        if ((nbApp - 1) <= 0)
        {
            // give permission to app
            request.setPerformative(ACLMessage::ACCEPT_MUTEX);

            QJsonArray apps;
            apps.append(timestamp->getSiteID());

            QJsonObject content;
            content[QLatin1String("apps")] = apps;

            request.setContent(content);

            communicationMngr->send(request, QLatin1String("NET"), Header::allApp, Header::localHost);

            return;
        }
        else if (! localMutexWaitingList.contains(timestamp->getSiteID()))
        {
            localMutexWaitingList[timestamp->getSiteID()] = nbApp - 1;

            // mark message ID
            request.setSender(siteID);
            request.setNbSequence(++nbSequence);
        }
        else
        {
            return;
        }

        communicationMngr->send(request, QLatin1String("NET"), Header::allApp, Header::localHost);

        // Mark Snapshot marker because the request always comes before before the message

        if (snapshot)
        {
            QJsonObject contents = request.getContent();
            snapshot->colorMessage(contents);
            request.setContent(contents);
        }

        communicationMngr->send(request, QLatin1String("NET"), QLatin1String("NET"), Header::localHost);
    }
    else
    {
        communicationMngr->send(request, QLatin1String("NET"), QLatin1String("NET"), Header::localHost);

        if (snapshot)
        {
            QJsonObject contents = request.getContent();

            if (snapshot->getColor(contents))
            {
                // detected prepost message
                ACLMessage prepost = snapshot->encodePrepostMessage(request);
                prepost.setSender(siteID);
                prepost.setNbSequence(++nbSequence);
                forwardPrepost(prepost);
            }

            request.setContent(contents);
        }

        communicationMngr->send(request, QLatin1String("NET"), Header::allApp, Header::localHost);
    }
}

void Router::Private::receiveMutexApproval(ACLMessage& approval, bool fromLocal)
{    
    QJsonArray           approvedApps = approval.getContent()[QLatin1String("apps")].toArray();
    QJsonArray::iterator iter         = approvedApps.begin();

    while (iter != approvedApps.end())
    {
        if ( localMutexWaitingList.contains((*iter).toString()) )
        {
            if (--localMutexWaitingList[(*iter).toString()] == 0)
            {
                // give permission to app
                QJsonArray apps;
                apps.append((*iter));

                QJsonObject content;
                content[QLatin1String("apps")] = apps;
                approval.setContent(content);

                communicationMngr->send(approval, QLatin1String("NET"), Header::allApp, Header::localHost);

                localMutexWaitingList.remove((*iter).toString());
            }

            //qDebug() << siteID << "waiting list" << localMutexWaitingList;
            iter = approvedApps.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if (approvedApps.size() == 0)
    {
        return;
    }

    if (fromLocal)
    {
        // mark message ID
        approval.setSender(siteID);
        approval.setNbSequence(++nbSequence);
    }

    // update list of approvedApps
    QJsonObject content;
    content[QLatin1String("apps")] = approvedApps;

    approval.setContent(content);

    // forward to network
    communicationMngr->send(approval, QLatin1String("NET"), QLatin1String("NET"), Header::localHost);
}

void Router::Private::refuseAllPendingRequest()
{
    ACLMessage refuse(ACLMessage::REFUSE_MUTEX);
    QJsonArray apps = QJsonArray::fromStringList(localMutexWaitingList.keys());

    QJsonObject content;
    content[QLatin1String("apps")] = apps;

    refuse.setContent(content);

    communicationMngr->send(refuse, QLatin1String("NET"), Header::allApp, Header::localHost);

    localMutexWaitingList.clear();
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

    if ( recentSequences.contains(sender) && (recentSequences[sender] >= sequence) )
    {
        // Here we suppose the channels are FIFO
        // therefore for each router, by keeing the sequence number of each site, we can identify old repeated message

        return true;
    }

    // Update recent sequence
    recentSequences[sender] = sequence;

    return false;
}

/*------------------------------------------------------------ Router main functions ----------------------------------------------------------------------------------------*/


Router::Router(CommunicationManager* communication, const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    if (!communication)
    {
        qFatal("Router: Communication Manager is null");
    }

    setObjectName(QLatin1String("Router"));

    d->communicationMngr = communication;
    d->siteID            = siteID;
    d->watchdog          = new Watchdog(siteID);

    connect(d->communicationMngr, SIGNAL(signalMessageReceived(Header, Message)),
            this,                 SLOT(slotReceiveMessage(Header, Message)), Qt::DirectConnection);

    connect(d->watchdog, &Watchdog::signalPingLocalApps,
            this,        &Router::slotBroadcastLocal, Qt::DirectConnection);

    connect(d->watchdog, &Watchdog::signalSendInfo,
            this,        &Router::slotBroadcastNetwork, Qt::DirectConnection);

    connect(d->watchdog, &Watchdog::signalNetworkChanged,
            this,        &Router::slotUpdateNbApps, Qt::DirectConnection);
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
            this,        &Router::slotBroadcastLocal, Qt::DirectConnection);

    connect(d->snapshot, &LaiYangSnapshot::signalSendSnapshotMessage,
            this,        &Router::slotBroadcastNetwork, Qt::DirectConnection);

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

            case ACLMessage::PING:
                d->forwardPing(aclMessage);
                break;

            case ACLMessage::PONG:
                d->forwardPong(aclMessage, false);
                break;

            case ACLMessage::REQUEST_MUTEX:
                d->receiveMutexRequest(aclMessage, false);
                break;

            case ACLMessage::ACCEPT_MUTEX:
                d->receiveMutexApproval(aclMessage, false);
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
                d->forwardPong(aclMessage, true);
                break;

            case ACLMessage::REQUEST_MUTEX:
                d->receiveMutexRequest(aclMessage, true);
                break;

            case ACLMessage::ACCEPT_MUTEX:
                d->receiveMutexApproval(aclMessage, true);
                break;

            default:
                d->forwardAppToNet(header, aclMessage);
                break;
        }
    }
}

void Router::slotBroadcastLocal(const Message& message)
{
    d->communicationMngr->send(message, QLatin1String("NET"), Header::allApp, Header::localHost);
}

void Router::slotBroadcastNetwork(ACLMessage& message)
{
    message.setSender(d->siteID);
    message.setNbSequence(++d->nbSequence);

    d->communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}

void Router::slotUpdateNbApps(int nbSites, int nbApp)
{
    if (nbApp != d->nbApp)
    {
        qDebug() << d->siteID << ": network changed => restart all mutex";
        // In case the network structure changed, it should restart the process of mutex in order to avoid deadlock
        d->refuseAllPendingRequest();
    }

    d->nbApp = nbApp;

    d->snapshot->setNbOfApp(nbApp);
    d->snapshot->setNbOfNeighbor(nbSites - 1);
}

}
