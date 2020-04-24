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

#ifndef LAI_YANG_SNAPSHOT_H
#define LAI_YANG_SNAPSHOT_H

// Qt include
#include <QObject>

// Local include
#include "message.h"

namespace AirPlug
{

class LaiYangSnapshot : public QObject
{
    Q_OBJECT
public:

    LaiYangSnapshot();
    ~LaiYangSnapshot();

    // Message type: common message
    static const QString commonType;

public:

    /**
     * @brief init snapshot action, this method is called only one time by initiator
     */
    void init();

    /**
     * @brief colorMessage: append additional color field to the message from Base application before sending
     * @param message
     */
    void colorMessage(Message& message);

    /**
     * @brief preprocessMessage: snapshot guard verification after message arrival
     * @param message
     * @return
     */
    Message preprocessMessage(const Message& message);

private:

    class Private;
    Private* d;
};

}
#endif // LAI_YANG_SNAPSHOT_H
