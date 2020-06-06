/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-28
 * Description : Awerbuch’s beta Synchronizer for a synchronous system, control part
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * 2020 by Mathieu Cocheteux <mathieu.cocheteux@etu.utc.fr>
 *
 * ============================================================ */

#ifndef SYNCHRONIZER_CONTROL_H
#define SYNCHRONIZER_CONTROL_H

// Qt includes
#include <QObject>

// Local includes
#include "aclmessage.h"
#include "election_manager.h"

namespace AirPlug
{

/**
 * @brief The Synchronizer class implement Awerbuch’s β Synchronization algorithm
 * The key idea is to include an initialization phase, in which a centralized wave algorithm is employed to build a
 * spanning tree of the network. The safe messages travel up the tree to the root. When the root of the
 * tree has received safe messages from all its children, all processes have become safe, so that all
 * processes can move to the next pulse. Then next messages travel down the tree to start this next pulse.
 *
 * An object Synchronizer is placed in NET application, if NET receives a message with performative SYNC (synchronized message),
 * if the synchronization process hasn't been initaied, Synchronizer will call for an election to chose a unique initiator.
 * Synchronizer marks the Base application who send out the first message, this BAS will be the controller of the sync network in case
 * Synchronizer wins the election.
 *
 * The first sync message from BAS should only be a dummy to init the synchronization
 *
 * The implementation of this algo will be devided in to 2 components:
 *  - One with the base application (BAS) :
 *      + helps base application mark messages as sync messages,
 *      + receive messages and apply some controls to keep the system in synchronization
 *      + receive notify cation from the Control part in Control application at each step
 *  - One with the control application (NET) :
 *      + helps init the synchronization
 *      + Forward messages between Base applications
 *      + Apply control of ACK message to keep system in sync
 */
class SynchronizerControl: public QObject
{
    Q_OBJECT
public:

    explicit SynchronizerControl(const QString& siteID);
    ~SynchronizerControl();

public:

    /**
     * @brief init: make local site become the initiator of the network
     * Election will be used to elect one unique initiator per network
     */
    void init();

    /**
     * @brief processLocalMessage : process messages coming from local base application
     * @param message
     * @return true if message can continue to be send, false when it takes a different route
     */
    bool processLocalMessage(ACLMessage& message);

    /**
     * @brief processExternalMessage : process messages coming from network
     * @param message
     * @return true if message can continue to be send, false when it takes a different route
     */
    bool processExternalMessage(ACLMessage& message);

    void setNbOfApp(int nbApps);

public:

    // Use for forwarding message from local base application to the network
    Q_SIGNAL void signalSendToNet(ACLMessage& message);

    // Use for forwarding message from network to local base application
    Q_SIGNAL void signalSendToApp(ACLMessage& message);

    // signal sent to router to request an election
    Q_SIGNAL void signalRequestElection(ElectionManager::ElectionReason reason);
    Q_SIGNAL void signalFinishElection(ElectionManager::ElectionReason reason);

private:

    /**
     * @brief callElection : call for new election from synchronizer
     * @param baseID
     */
    void callElection(const QString& baseID);

private:

    class Private;
    Private* d;
};

}
#endif // SYNCHRONIZER_CONTROL_H
