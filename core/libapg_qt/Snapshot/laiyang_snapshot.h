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

    // TODO integrate with Wave
    enum ForwardPort
    {
        BAS = 0,        // forward to Basic application
        NET,            // forward to the network
        DROP,           // drop the message
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
     */
    void colorMessage(QJsonObject& messageContent);

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
    bool processPrePostMessage(ACLMessage& message);

public:

    // NOTE: these signals have to be connect by Qt::DirectConnection to invoke the slot immediately
    Q_SIGNAL void signalRequestSnapshot(const Message* marker);         // send to BAS

private:

    /**
     * @brief requestSnapshot: with the hypothesis that channel between Base application and Control application is FIFO,
     * this function send a request to Base application to take a snapshot
     */
    void requestSnapshot();

    //TODO: implement condition of termination after implementing wave
private:

    class Private;
    Private* d;
};

}
#endif // LAI_YANG_SNAPSHOT_H
