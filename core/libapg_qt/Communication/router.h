/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-27
 * Description : Handler for Message routing at NET layer
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef ROUTER_H
#define ROUTER_H

// libapg include
#include "aclmessage.h"
#include "communication_manager.h"
#include "laiyang_snapshot.h"

namespace AirPlug
{

class Router : public QObject
{
    Q_OBJECT
public:

    Router(CommunicationManager* communication, const QString& siteID);
    ~Router();

    bool addSnapshot(LaiYangSnapshot* snapshot);

public:

    /**
      * @brief slotReceiveMessage :  main event handler
      */
    Q_SLOT void slotReceiveMessage(Header, Message);

    /**
      * @brief slotSendMarker : broadcast marker to local applications to take snapshot
      * @param marker
      */
    Q_SLOT void slotSendMarker(const Message& marker);

    Q_SLOT void slotForwardSnapshotMessage(ACLMessage& message);

    /**
      * @brief slotHealthCheck : broadcast PING message to local applications
      *
      */
    Q_SLOT void slotHeathCheck();

private:

    Q_SLOT void slotPingTimeOut();
    Q_SLOT void slotRefreshActiveNeighbor();

private:

    class Private;
    Private* d;
};

}
#endif // ROUTER_H
