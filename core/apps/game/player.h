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

// Local include

//using namespace AirPlug;

namespace GameApplication
{

typedef struct
{
    bool left;
    bool right;
    bool up;
    bool down;
} Controls;

class Player : public QGraphicsRectItem
{
    //Q_OBJECT
private:
    Controls controls = {false,false,false,false};

    int x_speed = 0;
    int y_speed = 0;

public:
    Player();
    ~Player();

    void keyPressEvent(QKeyEvent *event);
};

}
#endif // PLAYER_H
