/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-2
 * Description : implementation of Raymond's mutual exclusion algorithm
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef RAYMOND_LOCK_H
#define RAYMOND_LOCK_H

namespace AirPlug
{

class RaymondLock
{
public:

    RaymondLock();
    ~RaymondLock();

private:

    class Private;
    Private* d;
};


}
#endif // RAYMOND_LOCK_H
