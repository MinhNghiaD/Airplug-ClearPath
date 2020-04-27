/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-27
 * Description : broadcast handler using vector clock to filter out redundant messages
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef BROADCAST_HANDLER_H
#define BROADCAST_HANDLER_H

// libapg include
#include "aclmessage.h"

namespace AirPlug
{

class BroadcastHandler
{
public:

    BroadcastHandler();
    ~BroadcastHandler();

private:

    class Private;
    Private* d;
};
}
#endif // BROADCAST_HANDLER_H
