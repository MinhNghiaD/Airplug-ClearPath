/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-23
 * Description : implementation of vector clock
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef VECTOR_CLOCK_H
#define VECTOR_CLOCK_H

namespace AirPlug
{

class VectorClock
{
public:

    VectorClock();
    ~VectorClock();

private:

    class Private;
    Private* d;
};

}
#endif // VECTOR_CLOCK_H
