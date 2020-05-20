/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-20
 * Description : Manager of Election process in the network
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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
 * @brief The ElectionManager class manage election processes of a site
 */
class ElectionManager : public QObject
{
    Q_OBJECT
public:

    enum ElectionReason
    {
        Snapshot = 0,
    };

public:

    explicit ElectionManager(const QString& siteID);
    ~ElectionManager();

    // take request from local
    void requestElection(ElectionReason reason);

    void finishElection(ElectionReason reason);

public:

    // receive request from network
    Q_SLOT void slotReceiveElectionRequest(ACLMessage& request);

    // receive ack message from network
    Q_SLOT void slotReceiveElectionAck(ACLMessage& ackMessage);

    // receive finish message from network
    Q_SLOT void slotReceiveFinishElection(ACLMessage& finishMessage);


public:

    Q_SIGNAL void signalSendElectionMessage(Message& request);
    Q_SIGNAL void signalWinElection();

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
