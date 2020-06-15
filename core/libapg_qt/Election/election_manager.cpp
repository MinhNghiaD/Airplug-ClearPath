#include "election_manager.h"

// Qt includes
#include <QHash>
#include <QDebug>

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
    int     nbNeighbor;

    // Map ongoing election with the siteID of current potential elected candidate
    QHash<ElectionReason, Election> ongoingElections;
};

/* ***************** ElectionManager Main *************************** */

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

void ElectionManager::initElection(ElectionReason reason)
{
    if (d->ongoingElections.contains(reason))
    {
        return;
    }
    else
    {
        // Starting the election process
        // if not already in an election => save election to the list of ongoing election
        // nbWaitedResponses is set to the nbNeighbor
        Election election;
        election.reason            = reason;
        election.candidate         = d->siteID;
        election.nbAgains          = 0;
        election.nbFor             = 0;
        election.nbWaitedResponses = d->nbNeighbor;

        d->ongoingElections.insert(reason, election);

        ACLMessage request(ACLMessage::ELECTION);

        QJsonObject content;
        content.insert(QLatin1String("reason"), reason);
        content.insert(QLatin1String("candidate"), d->siteID);

        request.setContent(content);

        // Broadcast election message
        qDebug() << "ELECTION CANDIDATE :" << d->siteID << "candidate for election with reason:" << reason;
        emit signalSendElectionMessage(request);
    }
}

void ElectionManager::processElectionRequest(ACLMessage& request)
{
    QJsonObject content = request.getContent();
    QString candidate = content[QLatin1String("candidate")].toString();

    // process request message and update ongoing elections
    ElectionReason reason = static_cast<ElectionReason>(content[QLatin1String("reason")].toInt());

    if (d->ongoingElections.contains(reason))
    {
        // Already a candidate or a 2nd+ call of a non candidate
        if (d->ongoingElections[reason].candidate > candidate)
        {
            // Update candidate voted by this site
            d->ongoingElections[reason].candidate = candidate;
        }
    }
    else
    {
        // First time : generally not a candidate
        Election election;
        election.candidate         = candidate;
        election.reason            = reason;
        election.nbFor             = 0;
        election.nbAgains          = 0;
        election.nbWaitedResponses = d->nbNeighbor;

        d->ongoingElections.insert(reason, election);
    }

    // Send ACK message back to sender
    request.setPerformative(ACLMessage::ACK_ELECTION);
    request.setReceiver(request.getSender());

    QJsonObject responseContent;
    // indicate siteID of current candidate of this site and send back to sender
    responseContent[QLatin1String("candidate")] = d->ongoingElections[reason].candidate;
    responseContent[QLatin1String("reason")]    = d->ongoingElections[reason].reason;

    request.setContent(responseContent);

    emit signalSendElectionMessage(request);
}

void ElectionManager::processElectionAck(ACLMessage& ackMessage)
{
    QJsonObject    content   = ackMessage.getContent();
    QString        candidate = content[QLatin1String("candidate")].toString();
    ElectionReason reason    = static_cast<ElectionReason>(content[QLatin1String("reason")].toInt());

    // If candidate currently voted by ack message is local site => increment nbFor
    // If candidate currently voted by ack message is not local site => increment nbAgains
    // Decrement nbWaitedResponses, if nbWaitedResponses = 0 => election finish
    // if nbFor == nbNeighbor => win election, send signal signalWinElection back to Router to give permission to do demanded task
/*
    if (d->ongoingElections[reason].nbWaitedResponses == 0)
    {
        return;
    }
*/
    if (candidate == d->siteID)
    {
        d->ongoingElections[reason].nbFor += 1;
    }
    else
    {
        d->ongoingElections[reason].nbAgains += 1;
    }

    // Le site élu a reçu tous les votes "Pour" de ses voisins
    if (d->ongoingElections[reason].nbFor    == d->ongoingElections[reason].nbWaitedResponses &&
        d->ongoingElections[reason].nbAgains == 0)
    {
        qDebug() << "ELECTION WIN :" << d->siteID << "win election for election reason :" << reason;
        emit signalWinElection(reason);
    }
}

void ElectionManager::finishElection(ElectionReason reason)
{
    if (d->siteID == d->ongoingElections[reason].candidate)
    {
        // only winner can broadcast finish election message
        ACLMessage inform(ACLMessage::FINISH_ELECTION);
        // mark reason of finished election
        QJsonObject content;
        content[QLatin1String("reason")] = reason;

        inform.setContent(content);

        // Broadcast election message
        emit signalSendElectionMessage(inform);
    }

    d->ongoingElections.remove(reason);
}

void ElectionManager::setNbOfNeighbor(int nbNeighbor)
{
    if (d->nbNeighbor > nbNeighbor)
    {
        // Reverify condition of termination
        for (QHash<ElectionReason, Election>::iterator iter  = d->ongoingElections.begin();
                                                       iter != d->ongoingElections.end();
                                                     ++iter)
        {
            iter.value().nbWaitedResponses = nbNeighbor;

            if (iter.value().nbFor    == iter.value().nbWaitedResponses &&
                iter.value().nbAgains == 0)
            {
                qDebug() << "ELECTION WIN :" << d->siteID << "win election for election reason :" << iter.key();
                emit signalWinElection(iter.key());
            }
        }
    }

    d->nbNeighbor = nbNeighbor;
}

}
