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

public:

    State state;
};

Player::Player()
    : d(new Private())
{
}

Player::~Player()
{
    delete d;
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

void Player::setSpeed(int xSpeed, int ySpeed)
{
    d->state.xSpeed = xSpeed;
    d->state.ySpeed = ySpeed;
}

void Player::setState(const State& state)
{
    d->state = state;
}

void Player::incrementFrame(void)
{
    d->state.frame++;
}

}
