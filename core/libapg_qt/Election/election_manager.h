/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-20
 * Description : Manager of Election process in the network
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * Contributors :
 * Gaétan Carabetta <carabetta.gaetan at gmail.com>
 *
 * ============================================================ */

#ifndef ELECTION_MANAGER_H
#define ELECTION_MANAGER_H

#include <QObject>

// local includes
#include "aclmessage.h"

namespace AirPlug
{

/**
 * @brief The ElectionManager class manages election processes of a site
 * Election is used for chosing a unique initiator in a use case where the network need a unique initiator.
 * At every point in time, there is always at most one ongoing election for each reason
 *
 * The mechanism of election in a network is described as below:
 *
 *  - When ever a site need to call for an election, it check if there is already an ongoing election for the same reason,
 *      + if not, it will save information of the election and broadcast election message to all sites
 *      + if there already is an election, it backs down
 *
 *  - When a site receive an election message, it will update information about that election in the list of ongoing elections:
 *      + If the siteID of the sender is smaller that the siteID of the currect candidate, it will replace the current candidate with the siteID of sender.
 *        Then send back an ACK Message with the siteID of the current candidate
 *      + If the siteID of sender is greater, it will keep the current candidate at the one it vote for
 *        and send back an ACK message with the siteID of the current candidate
 *
 *      => Therefore, in the case where there are multiple candidates enter the election process,
 *         there is always only one candidate who receives the accord from all site, this one will be decided to win the election
 *
 *  - When a site receive an ACK message, it update the information about the election by:
 *      + Decrement the number of waiting responses
 *      + See if ACK message is vote for who, update the number of FOR and AGAINST
 *      + When the number of waiting responses reachs 0, if the number of FOR vote is equal to the number of sites then it wins the election
 *
 *  - When a site wins an election, it can enter a demanding task where it becomes the initiator.
 *    When the task is finished, the site broadcast out a FINISH_ELECTION message to inform all sites that the election is finished
 *    and it can be remove from the list of ongoing elections
 */
class ElectionManager : public QObject
{
    Q_OBJECT
public:

    // Maybe more in the future
    enum ElectionReason
    {
        Snapshot = 0,
    };

public:

    explicit ElectionManager(const QString& siteID);
    ~ElectionManager();

public:

    /**
     * @brief requestElection: take request from hosted site
     * @param reason
     */
    void requestElection(ElectionReason reason);

    /**
     * @brief finishElection: finish election informed by hosted site
     * @param reason
     */
    void finishElection(ElectionReason reason);

    /**
     * @brief setNbOfNeighbor :  set number of site in the network
     * @param nbNeighbor
     */
    void setNbOfNeighbor(int nbNeighbor);

    /**
     * @brief processElectionRequest: process an election request received from network
     * @param request
     */
    void processElectionRequest(ACLMessage& request);

    /**
     * @brief processElectionAck: process an election ack message received from network
     * @param ackMessage
     */
    void processElectionAck(ACLMessage& ackMessage);

    /**
     * @brief processFinishElection: receive finish message from network
     * @param finishMessage
     */
    void processFinishElection(ACLMessage& finishMessage);


public:

    Q_SIGNAL void signalSendElectionMessage(ACLMessage& request);
    Q_SIGNAL void signalWinElection(ElectionReason reason);

private:

    class Private;
    Private* d;
};

/**
 * @brief The Election class contents data of an ongoing election
 */
class Election
{
public:

    explicit Election()
    {
    }

public:

    ElectionManager::ElectionReason reason;
    // the current potential candidate
    QString candidate;

    int nbFor;
    int nbAgains;
    int nbWaitedResponses;
};

}
#endif // ELECTION_MANAGER_H
