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
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QBrush>
#include <QTimer>
#include <QLinearGradient>
#include <QColor>

//std includes
#include <vector>
#include <memory>
#include <cmath>
#include <random>
#include <QList>

// Local include
#include "player.h"
#include "constants.h"

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
    std::vector<std::shared_ptr<Player>> connected_player;

    QTimer frame_timer;

    void moveAndUpdatePlayer(Player &player);
    void fixCollisions(Player &player);

public slots:
    void frameTimeout(void);

public:
    World();
    ~World();
};

}
#endif // WORLD_H
