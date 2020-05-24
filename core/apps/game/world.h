/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-24
 * Description : game world class
 *
 * 2020 by Lucca Rawlyk
 *
 * ============================================================ */

#ifndef WORLD_H
#define WORLD_H

//qt includes
#include <QObject>
#include <QCoreApplication>

namespace GameApplication
{

class World : public QObject
{
    Q_OBJECT

public:

    World(QCoreApplication &app);
    ~World();

private Q_SLOTS:

    void frameTimeout(void);
    void playerUpdate(int playerIndex, QString payerState);

private:

    class Private;
    Private* d;
};

}
#endif // WORLD_H
