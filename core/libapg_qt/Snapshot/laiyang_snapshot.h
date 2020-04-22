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

#ifndef LAIYANG_SNAPSHOT_H
#define LAIYANG_SNAPSHOT_H

namespace AirPlug
{

class LaiYangSnapshot
{
public:

    LaiYangSnapshot();
    ~LaiYangSnapshot();

private:

    class Private;
    Private* d;
};

}
#endif // LAIYANG_SNAPSHOT_H
