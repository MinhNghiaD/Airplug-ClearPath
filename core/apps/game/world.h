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

// Qt include
//#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QBrush>
#include <QTimer>

//std includes
#include <vector>
#include <memory>
#include <utility>

// Local include
#include "player.h"

//using namespace AirPlug;

namespace GameApplication
{

class World : public QObject
{
    Q_OBJECT

private:
    QGraphicsScene scene;
    QGraphicsView view;

    Player main_player;
    std::vector<std::unique_ptr<Player>> connected_players;

    QTimer frame_timer;

public slots:
    void frameTimeout();

public:
    World();
    ~World();
};

}
#endif // WORLD_H
