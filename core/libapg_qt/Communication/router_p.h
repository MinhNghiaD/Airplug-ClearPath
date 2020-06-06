/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-27
 * Description : Private implementation of router
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * 2020 by Gaétan Carabetta <carabetta.gaetan at gmail dot com>
 *
 * ============================================================ */

#ifndef ROUTER_P_H
#define ROUTER_P_H

#include "router.h"

#include "watchdog.h"
#include "synchronizer_control.h"

namespace AirPlug
{

class Q_DECL_HIDDEN Router::Private
{
public:

    explicit Private();
    ~Private();

public:

    void forwardAppToNet(Header& header, ACLMessage& message);
    void forwardNetToApp(Header& header, ACLMessage& message);

    void forwardStateMessage (ACLMessage& message, bool fromLocal);
    void forwardPrepost(const ACLMessage& message);
    void forwardRecover(const ACLMessage& message);
    void forwardReady  (const ACLMessage& message);
    void forwardPing   (const ACLMessage& message);
    void forwardPong   (const ACLMessage& message, bool fromLocal);

    void receiveElectionMsg  (ACLMessage& message);
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

    ElectionManager*      electionMng;
    SynchronizerControl*  synchronizer;

    // map external router with : - the most recent nbSequence received
    QHash<QString, int>   recentSequences;

    // each NET will keep track of its applications request
    QHash<QString, int>   localMutexWaitingList;
};

}

#endif // ROUTER_P_H
