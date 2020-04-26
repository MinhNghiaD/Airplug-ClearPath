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

// Local include
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

    LaiYangSnapshot();
    ~LaiYangSnapshot();

public:

    /**
     * @brief init snapshot action, this method is called only one time by initiator
     */
    void init();

    /**
     * @brief processMessage : process a new message before forwarding it
     * @param message
     * @param isLocal
     * @return :
     *      - true : accept to forward message
     *      - false: drop message
     */
    bool processMessage(ACLMessage* message, bool isLocal);

public:

    // NOTE: this signal has to be connect by Qt::DirectConnection to envoke the slot immediately
    Q_SIGNAL void signalRequestSnapshot(Message* marker);

private:

    /**
     * @brief requestSnapshot: with the hypothesis that channel between Base application and Control application is FIFO,
     * this function send a request to Base application to take a snapshot
     */
    void requestSnapshot();

    /**
     * @brief colorMessage: append additional color field to the message from Base application before sending them
     * @param message
     */
    void colorMessage(ACLMessage* message);

    /**
     * @brief getColor : get color of incomming message
     * @param message
     * @return
     */
    QString getColor(ACLMessage* message) const;

    /**
     * @brief processStateMessage : action taken when receive an ACL message with performative INFORM_STATE
     * @param message
     * @param isLocal
     * @return
     */
    bool processStateMessage(ACLMessage* message, bool isLocal);

    /**
     * @brief processPrePostMessage : action taken when receive an ACL Message with performative PREPOST_MESSAGE
     * @param message
     * @return
     */
    bool processPrePostMessage(ACLMessage* message);


    /**
     * @brief collectState: collect a local state
     * @param state
     *
     * A State object should have the form of :
     * {
     *     siteID : Uuid
     *     clock  : vector clock
     *     state  : {
     *                  options: application option
     *                  local varable : jsonObject
     *              }
     * }
     *
     */
    void collectState(const QJsonObject& state);

    /**
     * @brief validateState :  verify if a state is valide to record
     * @param state
     * @return
     */
    bool validateState(const QJsonObject& state);

private:

    class Private;
    Private* d;
};

}
#endif // LAI_YANG_SNAPSHOT_H
