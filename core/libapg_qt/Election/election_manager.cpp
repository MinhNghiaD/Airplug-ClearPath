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
    int     nbNeighbor;

    // Map ongoing election with the siteID of current potential elected candidate
    QHash<ElectionReason, Election> ongoingElections;
  };

  /* ***************** ElectionManager Main ************* */
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
      // Starting the election process
      // TODO ELECTION 1: if not already in an election => save election to the list of ongoing election
      // nbWaitedResponses is set to the nbNeighbor

      // Maybe *Election instead of Election
      Election* election = new Election();
      election->reason = reason;
      election->candidate = d->siteID;
      election->nbAgains = 0;
      election->nbFor = 0;
      election->nbWaitedResponses = d->nbNeighbor;

      d->ongoingElections.insert(reason, *election);


      ACLMessage request(ACLMessage::ELECTION);

      // TODO ELECTION 2: write request message to QJsonObject indicate Election Reason and siteID then broadcast
      QJsonObject content;
      QJsonArray array;
      array["reason"] = reason;
      array["siteID"] = d->siteID;
      content.insert("election", array);

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
    if (content.contains("reason") && content["reason"] == ElectionReason::Snapshot){
      if (content.contains("candidate") && content["candidate"].toInt() < d->siteID) {
        d->ongoingElections[content["reason"]].nbAgains += 1;
      } else {
        d->ongoingElections[content["reason"]].nbFor += 1;
      }
      d->ongoingElections[content["reason"]].nbWaitedResponses -= 1;
    } else {
      d->ongoingElections[content["reason"]].nbWaitedResponses = 0;
    }

    // Send ACK message back to sender
    request.setPerformative(ACLMessage::ACK_ELECTION);
    request.setReceiver(request.getSender());

    QJsonObject responseContent;
    // TODO ELECTION 4: indicate siteID of current candidate of this site and send back to sender
    responseContent;


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
      ElectionReason reason;

      emit signalWinElection(reason);
    }
  }

  void ElectionManager::finishElection(ElectionReason reason)
  {
    if (d->siteID == d->ongoingElections[reason].candidate)
    {
      // only winner can broadcast finish election message
      ACLMessage inform(ACLMessage::FINISH_ELECTION);

      // Broadcast election message
      emit signalSendElectionMessage(inform);
    }

    d->ongoingElections.remove(reason);
  }


  void ElectionManager::processFinishElection(ACLMessage& finishMessage)
  {
    QJsonObject content = finishMessage.getContent();
    // TODO ELECTION 8: get reason of finish election in order to remove from ongoing list

    //d->ongoingElections.remove(reason);
  }

  void ElectionManager::setNbOfNeighbor(int nbNeighbor)
  {
    d->nbNeighbor = nbNeighbor;
  }

}
