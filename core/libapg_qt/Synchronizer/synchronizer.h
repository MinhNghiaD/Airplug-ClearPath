/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-28
 * Description : Awerbuch’s beta Synchronizer for a synchronous system
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

// Qt includes
#include <QObject>

// Local includes

namespace AirPlug
{

class Synchronizer: public QObject
{
    Q_OBJECT
public:

    explicit Synchronizer();
    ~Synchronizer();

private:

    class Private;
    Private* d;
};

}
#endif // SYNCHRONIZER_H
