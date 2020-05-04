#include "player.h"

//qt includes
//#include <QDebug>

//std includes
#include <cmath>

//local include
#include "constants.h"

namespace GameApplication
{

class Q_DECL_HIDDEN Player::Private
{
public:
    Private()
    {

    }
    ~Private()
    {

    }

    State state;
};

Player::Player() : d(std::make_unique<Private>())
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
        d->state.left = true;
        break;

        case Qt::Key_Right:
        d->state.right = true;
        break;

        case Qt::Key_Up:
        d->state.up = true;
        break;

        case Qt::Key_Down:
        d->state.down = true;
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
        d->state.left = false;
        break;

        case Qt::Key_Right:
        d->state.right = false;
        break;

        case Qt::Key_Up:
        d->state.up = false;
        break;

        case Qt::Key_Down:
        d->state.down = false;
        break;

        default:
        return;
    }
}

State Player::getState(void)
{
    d->state.x = x();
    d->state.y = y();
    return d->state;
}

int Player::getFrame(void)
{
    return d->state.frame;
}

void Player::setSpeed(int x_speed, int y_speed)
{
    d->state.x_speed = x_speed;
    d->state.y_speed = y_speed;
}

void Player::setState(State _state)
{
    d->state = _state;
}

void Player::incrementFrame(void)
{
    d->state.frame++;
}

}
