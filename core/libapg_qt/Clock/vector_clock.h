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
#include <QByteArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace AirPlug
{

class VectorClock
{
public:

    VectorClock(const QString&     siteID);
    VectorClock(const QString&     siteID, const QHash<QString, int>& vector);
    VectorClock(const QJsonObject& json);
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

    /**
     * @brief updateClock: update local clock after receive another's clock
     * @param other
     */
    void updateClock(const VectorClock& other);

    /**
     * @brief operator < : order relation between 2 clocks
     * @param other
     * @return
     */
    bool operator < (const VectorClock& other) const;

public:

    /**
     * @brief convertToJson: turn Clock to Json format
     * @return
     */
    QJsonObject convertToJson() const;

    /**
     * @brief getSiteID: return siteID
     * @return
     */
    QString getSiteID() const;

    /**
     * @brief getValue : return decimal clock value of a particular site
     * @param siteID
     * @return
     */
    int getValue(const QString& siteID) const;

    /**
     * @brief siteLists : return the list of siteIDs noted in the clock
     * @return
     */
    QStringList siteLists() const;

    /**
     * @brief sum : use as a alternative solution for Lamport clock, if a -> b => sum(a) < sum(b), if a || b => we can organize a and b by an order uniform
     * @return
     */
    int sum() const;

    /**
     * @brief isGeneralSmaller :  comparation in total order of clocks using Lamport method
     * @param other
     * @return
     */
    bool isGeneralSmallerThan(const VectorClock& other);

private:

    class Private;
    Private* d;
};

}
#endif // VECTOR_CLOCK_H
