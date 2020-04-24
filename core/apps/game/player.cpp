#include "player.h"

//Qt includes
//#include <QDebug>

//using namespace AirPlug;

namespace GameApplication
{

Player::Player()
{

}

Player::~Player()
{

}

void Player::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Left:
        state.left = true;
        break;

        case Qt::Key_Right:
        state.right = true;
        break;

        case Qt::Key_Up:
        state.up = true;
        break;

        case Qt::Key_Down:
        state.down = true;
        break;

        default:
        return;
    }
}

void Player::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Left:
        state.left = false;
        break;

        case Qt::Key_Right:
        state.right = false;
        break;

        case Qt::Key_Up:
        state.up = false;
        break;

        case Qt::Key_Down:
        state.down = false;
        break;

        default:
        return;
    }
}

State Player::getState(void)
{
    state.x = x();
    state.y = y();
    return state;
}

void Player::setSpeed(int x_speed, int y_speed)
{
    state.x_speed = x_speed;
    state.y_speed = y_speed;
}

}
