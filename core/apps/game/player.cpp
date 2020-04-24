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
        setPos(x()-5,y());
        break;

        case Qt::Key_Right:
        setPos(x()+5,y());
        break;

        case Qt::Key_Up:
        setPos(x(),y()-5);
        break;

        case Qt::Key_Down:
        setPos(x(),y()+5);
        break;

        default:
        return;
    }
}

}
