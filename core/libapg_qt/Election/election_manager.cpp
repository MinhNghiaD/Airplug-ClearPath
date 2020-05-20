#include "election_manager.h"

// Qt includes
#include <QHash>

namespace AirPlug
{
class Q_DECL_HIDDEN ElectionManager::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
    }

public:

    QString siteID;
    int     nbApp;

    // Map ongoing election with the siteID of current potential elected candidate
    QHash<ElectionReason, Election> ongoingElections;
};

ElectionManager::ElectionManager(const QString& siteID)
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Election Manager"));

    d->siteID = siteID;
}

ElectionManager::~ElectionManager()
{
    delete d;
}

void ElectionManager::requestElection(ElectionReason reason)
{
    if (d->ongoingElections.contains(reason))
    {
        return;
    }
    else
    {
        // TODO ELECTION 1: if not already in an election => save election to the list of ongoing election
        // nbWaitedResponses is set to the nbofApp -1



        ACLMessage request(ACLMessage::ELECTION);

        QJsonObject content;
        // TODO ELECTION 2: write request message to QJsonObject indicate Election Reason and siteID then broadcast


        request.setContent(content);

        // Broadcast election message
        emit signalSendElectionMessage(request);
    }
}

void ElectionManager::processElectionRequest(ACLMessage& request)
{
    QJsonObject content = request.getContent();
    // TODO ELECTION 3: process request message and update ongoing elections
    // if not participating in this election, nbWaitedResponses is set to 0


    // Send ACK message back to sender
    request.setPerformative(ACLMessage::ACK_ELECTION);
    request.setReceiver(request.getSender());

    QJsonObject responseContent;
    // TODO ELECTION 4: indicate siteID of current candidate of this site and send back to sender


    request.setContent(responseContent);

    emit signalSendElectionMessage(request);
}

void ElectionManager::processElectionAck(ACLMessage& ackMessage)
{
    QJsonObject content = ackMessage.getContent();
    // TODO ELECTION 5: get candidate of ack message and update info about ongoing elections
    // If candidate in ack message is local site => increment nbFor
    // If candidate in ack message is not local site => increment nbAgains
    // Decrement nbWaitedResponses, if nbWaitedResponses = 0 => election finish
    // if nbFor == nbApp => win election, send signal signalWinElection back to Router to give permission to do the task

    if (false)
    {
        emit signalWinElection();
    }
}

void ElectionManager::finishElection(ElectionReason reason)
{
    d->ongoingElections.remove(reason);

    ACLMessage inform(ACLMessage::FINISH_ELECTION);

    QJsonObject content;
    // TODO ELECTION 7: broadcast finish election message with ElectionReason to all site


    inform.setContent(content);

    // Broadcast election message
    emit signalSendElectionMessage(inform);
}


void ElectionManager::processFinishElection(ACLMessage& finishMessage)
{
    QJsonObject content = finishMessage.getContent();
    // TODO ELECTION 8: get reason of finish election in order to remove from ongoing list

    //d->ongoingElections.remove(reason);
}

void ElectionManager::setNbOfApp(int nbApp)
{
    d->nbApp = nbApp;
}

}
