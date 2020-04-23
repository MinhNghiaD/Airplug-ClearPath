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

    ~VectorClock();

public:

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

private:

    class Private;
    Private* d;
};

}
#endif // VECTOR_CLOCK_H
