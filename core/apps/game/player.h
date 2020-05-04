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

//qt includes
//#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>

//std includes
#include <memory>

//local includes
#include "state.h"

namespace GameApplication
{

class Player : public QGraphicsRectItem
{
public:
    Player();
    ~Player();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    State getState(void);
    int getFrame();
    void setSpeed(int x_speed, int y_speed);
    void setState(State _state);
    void incrementFrame(void);

private:
    class Private;
    std::unique_ptr<Private> d;
};

}
#endif // PLAYER_H
