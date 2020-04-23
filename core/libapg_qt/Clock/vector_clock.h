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

// Qt include
#include <QString>
#include <QHash>

namespace AirPlug
{

class VectorClock
{
public:

    VectorClock(const QString& siteID);
    VectorClock(const QString& siteID, const QHash<QString, int>& vector);
    VectorClock(const VectorClock& other);

    ~VectorClock();

public:

    VectorClock operator= (const VectorClock& other);

    /**
     * @brief operator ++: prefix ++ operator
     * @return
     */
    VectorClock operator++ ();

    /**
     * @brief operator ++: postfix ++ operator
     * @return
     */
    VectorClock operator++ (int);

    void updateClock(const VectorClock& other);

    /**
     * @brief operator < : order relation between 2 clocks
     * @param other
     * @return
     */
    bool operator < (const VectorClock& other);

private:

    class Private;
    Private* d;
};

}
#endif // VECTOR_CLOCK_H
