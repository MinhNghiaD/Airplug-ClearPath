/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-24
 * Description : game world class
 *
 * 2020 by Lucca Rawlyk
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef WORLD_H
#define WORLD_H

//qt includes
#include <QGraphicsView>
#include <QCoreApplication>

// local include
#include "board.h"

namespace GameApplication
{

class World : public QGraphicsView
{
    Q_OBJECT

public:

    World(const QCoreApplication &app, Board* board);
    ~World();

private Q_SLOTS:

    //void slotFrameTimeout(void);
    //void slotPlayerUpdate(int playerIndex, QString playerState);

private:

    class Private;
    Private* d;
};

}
#endif // WORLD_H
