/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-22
 * Description : implementation of Lai-Yang Snapshot algorithm
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef LAI_YANG_SNAPSHOT_H
#define LAI_YANG_SNAPSHOT_H

// Qt include
#include <QObject>

// libapg include
#include "aclmessage.h"

namespace AirPlug
{

/**
 * @brief The LaiYangSnapshot class : Intermediate layer to handle Lai-Yang snapshot
 */
class LaiYangSnapshot : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief The Status enum : current status of snapshot
     * The goal of the algorithm is to maintain, at every point in time, there should be at most 2 status exist in the system
     * Snapshot algorithm operates in the transition READY -> RECORDED
     * Wave algorithm operates in the transition RECORDED -> RECOVERING ->READY
     */
    enum Status
    {
        READY = 0,
        RECORDED,
        RECOVERING,
    };

public:

    LaiYangSnapshot();
    ~LaiYangSnapshot();

public:

    /**
     * @brief setNbOfApp : update total nb of application
     * @param nbApp
     */
    void setNbOfApp(int nbApp);

    /**
     * @brief setNbOfNeighbor : update nb of neighbor
     * @param nbNeighbor
     */
    void setNbOfNeighbor(int nbNeighbor);

    /**
     * @brief init snapshot action, this method is called only one time by initiator
     */
    void init();

    /**
     * @brief colorMessage: append additional color field to the message from Base application before sending them
     * @param messageContent
     * @param nbReceivers
     *
     * (broadcast => nbReceivers = 0)
     */
    void colorMessage(QJsonObject& messageContent, int nbReceivers = 0);

    /**
     * @brief getColor : get color of incomming message
     * @param messageContent
     * @return
     *      - true if prepost message detected
     *      - false if not
     */
    bool getColor(QJsonObject& messageContent);

    /**
     * @brief processStateMessage : action taken when receive an ACL message with performative INFORM_STATE
     * @param message
     * @param isLocal
     * @return
     */
    bool processStateMessage(ACLMessage& message, bool fromLocal);

    /**
     * @brief processPrePostMessage : action taken when receive an ACL Message with performative PREPOST_MESSAGE
     * @param message
     * @return
     */
    bool processPrePostMessage(const ACLMessage& message);

    /**
     * @brief encodePrepostMessage : encode a prepost message to send to initiator
     * @param message
     * @return
     */
    ACLMessage encodePrepostMessage(const ACLMessage& message);

    /**
     * @brief finishSnapshot : terminate snapshot procedure and refresh it
     */
    void finishSnapshot();

    /**
     * @brief processRecoveringMessage : action taken when receive a message inform a site is recovering after snapshot
     * @param message
     * @return
     */
    bool processRecoveringMessage(const ACLMessage& message);

    /**
     * @brief processReadyMessage : action taken when receive a message inform initiator ready for snapshot
     * @param message
     * @return
     */
    bool processReadyMessage(const ACLMessage& message);

public:

    // NOTE: these signals have to be connected by Qt::DirectConnection to invoke the slot immediately
    Q_SIGNAL void signalRequestSnapshot(const Message& marker);           // send to BAS
    Q_SIGNAL void signalSendSnapshotMessage(ACLMessage& message);         // send to NET

private:

    /**
     * @brief requestSnapshot: with the hypothesis that channel between Base application and Control application is FIFO,
     * this function send a request to Base application to take a snapshot
     */
    void requestSnapshot();

    /**
      * @brief saveSnapshot : save snapshot to file
      */
    void saveSnapshot() const;

private:

    class Private;
    Private* d;
};

}
#endif // LAI_YANG_SNAPSHOT_H
