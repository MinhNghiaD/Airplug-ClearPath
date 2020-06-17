#include "router_p.h"

#include <QThread>
#include <QDebug>

namespace AirPlug
{

Router::Private::Private()
    : communicationMngr(nullptr),
      nbSequence(0),
      nbApp(0),
      snapshot(nullptr),
      watchdog(nullptr),
      electionMng(nullptr),
      synchronizer(nullptr)
{
}

Router::Private::~Private()
{
    delete watchdog;
    delete electionMng;
    delete synchronizer;
}

void Router::Private::forwardAppToNet(Header& header, ACLMessage& message)
{

    if (synchronizer)
    {
        if (!synchronizer->processLocalMessage(message))
        {
            return;
        }
    }

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
    if (message.getReceiver() != siteID)
    {
        // broadcast the message to another net
        communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), header.where());
    }
    else
    {
        // TODO routing
    }

    if (synchronizer)
    {
        if (!synchronizer->processExternalMessage(message))
        {
            return;
        }
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

        watchdog->receivePong(isNewApp);

        return;
    }

    // Receive pong from network => update network info for watchdog
    watchdog->receiveNetworkInfo(message);

    // forward
    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);
}


void Router::Private::receiveElectionMsg(ACLMessage& message)
{
    if (electionMng == nullptr)
    {
        // Forward to other NETs
        communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);

        return;
    }

    ACLMessage::Performative performative = message.getPerformative();

    if (performative          == ACLMessage::ACK_ELECTION &&
        message.getReceiver() == siteID)
    {
        electionMng->processElectionAck(message);

        return;
    }

    // Forward to other NETs
    communicationMngr->send(message, QLatin1String("NET"), QLatin1String("NET"), Header::allHost);

    // process Election messages
    switch (performative)
    {
        case ACLMessage::ELECTION:
            electionMng->processElectionRequest(message);
            break;

        case ACLMessage::FINISH_ELECTION:
            electionMng->finishElection(static_cast<ElectionManager::ElectionReason>
                                        (message.getContent()[QLatin1String("reason")].toInt()));
            break;

        default:
            break;
    }
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
    QJsonArray approvedApps = approval.getContent()[QLatin1String("apps")].toArray();

    for (int i = 0; i < approvedApps.size(); ++i)
    {
        QString app = approvedApps[i].toString();

        if (localMutexWaitingList.contains(app))
        {
            qDebug() << app << "approved by" << approval.getSender();
            localMutexWaitingList[app] -= 1;

            if (localMutexWaitingList[app] == 0)
            {
                // give permission to app
                QJsonArray apps;
                apps.append(app);

                ACLMessage permission(ACLMessage::ACCEPT_MUTEX);
                QJsonObject content;
                content[QLatin1String("apps")] = apps;
                permission.setContent(content);

                communicationMngr->send(permission, QLatin1String("NET"), Header::allApp, Header::localHost);

                localMutexWaitingList.remove(app);
            }
        }
    }

    qDebug() << siteID << "waiting list" << localMutexWaitingList;

    if (fromLocal)
    {
        qDebug() << siteID << "send aproval to" << approvedApps;
        // mark message ID
        approval.setSender(siteID);
        approval.setNbSequence(++nbSequence);
    }

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

}
