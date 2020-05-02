/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-24
 * Description : game player class
 *
 * 2020 by Lucca Rawlyk
 *
 * ============================================================ */

#ifndef PLAYER_H
#define PLAYER_H

// Qt include
//#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>

//std includes
#include <cmath>

// Local include
#include "state.h"
#include "constants.h"

//using namespace AirPlug;

namespace GameApplication
{

class Player : public QGraphicsRectItem
{
    //Q_OBJECT
private:
    State state;

public:
    Player();
    ~Player();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    State getState(void);
    void setSpeed(int x_speed, int y_speed);
    void incrementFrame(void);
};

}
#endif // PLAYER_H
