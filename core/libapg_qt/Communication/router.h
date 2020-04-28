/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-27
 * Description : Handler for Message routing
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef ROUTER_H
#define ROUTER_H

// libapg include
#include "aclmessage.h"

namespace AirPlug
{

class Router
{
public:

    Router();
    ~Router();

private:

    class Private;
    Private* d;
};
}
#endif // ROUTER_H
