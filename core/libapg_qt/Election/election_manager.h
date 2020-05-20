/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-20
 * Description : Manager of Election process in the network
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef ELECTION_MANAGER_H
#define ELECTION_MANAGER_H

#include <QObject>

namespace AirPlug
{

class ElectionManager : public QObject
{
    Q_OBJECT
public:

    explicit ElectionManager();
    ~ElectionManager();

private:

    class Private;
    Private* d;
};
}
#endif // ELECTION_MANAGER_H
