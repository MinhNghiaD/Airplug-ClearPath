/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-28
 * Description : Awerbuch’s beta Synchronizer for a synchronous system, control part
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef SYNCHRONIZER_CONTROL_H
#define SYNCHRONIZER_CONTROL_H

// Qt includes
#include <QObject>

// Local includes

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

    explicit SynchronizerControl();
    ~SynchronizerControl();

public:

    /**
     * @brief init: make local site become the initiator of the network
     * Election will be used to elect one unique initiator per network
     */
    void init(const QString& initiator);

    void processLocalMessage(const QString& initiator);

    void setNbApps(int nbApps);

public:


private:

    class Private;
    Private* d;
};

}
#endif // SYNCHRONIZER_CONTROL_H
