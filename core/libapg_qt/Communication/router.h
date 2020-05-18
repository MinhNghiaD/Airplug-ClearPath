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
      * @brief slotBroadcastLocal : broadcast marker to local applications to take snapshot
      * @param message
      */
    Q_SLOT void slotBroadcastLocal(const Message& message);

    Q_SLOT void slotBroadcastNetwork(ACLMessage& message);

    Q_SLOT void slotUpdateNbApps(int nbSites, int nbApp);

private:

    class Private;
    Private* d;
};

}
#endif // ROUTER_H
