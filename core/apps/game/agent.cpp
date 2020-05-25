#include "agent.h"

//qt includes
//#include <QDebug>
#include <QBrush>

//std includes
#include <cmath>
#include <random>

//local include
#include "constants.h"

namespace GameApplication
{

class Q_DECL_HIDDEN Agent::Private
{
public:

    Private(const QString& siteID, qreal radius)
        : siteID(siteID),
          radius(radius)
    {
    }

    ~Private()
    {
    }

public:

    QString siteID;
    State state;
    qreal radius;
};

Agent::Agent(const QString& siteID, qreal radius)
    : QGraphicsEllipseItem(0, 0, radius, radius),
      d(new Private(siteID, radius))
{
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setBrush(QBrush(Qt::green));

    // Init position of player randomly
    std::random_device seeder{};
    std::mt19937 twister{seeder()};
    std::uniform_int_distribution<> x(0, VIEW_WIDTH - 1 - PLAYER_SIZE);
    std::uniform_int_distribution<> y(0, VIEW_HEIGHT - 1 - PLAYER_SIZE);

    setPos(x(twister), y(twister));
}

Agent::~Agent()
{
    delete d;
}

void Agent::keyPressEvent(QKeyEvent *event)
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

void Agent::keyReleaseEvent(QKeyEvent *event)
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

State& Agent::getState(void)
{
    d->state.x = x();
    d->state.y = y();

    return d->state;
}

int Agent::getFrame(void)
{
    return d->state.frame;
}

void Agent::setState(const State& state)
{
    d->state = state;
}

void Agent::incrementFrame(void)
{
    d->state.frame++;
}

void Agent::move()
{
    //apply acceleration modifications from controls
    if(d->state.left == true)
    {
        d->state.xSpeed -= X_CONTROL_ACCELERATION;
    }
    else if(d->state.right == true)
    {
        d->state.xSpeed += X_CONTROL_ACCELERATION;
    }

    if(d->state.up == true)
    {
        d->state.ySpeed -= Y_CONTROL_ACCELERATION;
    }
    else if(d->state.down == true)
    {
        d->state.ySpeed += Y_CONTROL_ACCELERATION;
    }


    // update speed
    if(abs(d->state.xSpeed) > X_SPEED_LIMIT)
    {
        d->state.xSpeed = (d->state.xSpeed > 0) ? X_SPEED_LIMIT : (-X_SPEED_LIMIT);
    }

    if(abs(d->state.ySpeed) > Y_SPEED_LIMIT)
    {
        d->state.ySpeed = (d->state.ySpeed >= 0) ? Y_SPEED_LIMIT : (-Y_SPEED_LIMIT);
    }

    //apply speed reduction from friction
    if( (d->state.left == false && d->state.right == false) && d->state.xSpeed != 0 )
    {
        if(d->state.xSpeed > 0)
        {
            d->state.xSpeed -= X_FRICTION;

            if(d->state.xSpeed < 0)
            {
                d->state.xSpeed = 0;
            }
        }
        else
        {
            d->state.xSpeed += X_FRICTION;

            if(d->state.xSpeed > 0)
            {
                d->state.xSpeed = 0;
            }
        }
    }

    if( (d->state.up == false && d->state.down == false) && d->state.ySpeed != 0 )
    {
        if(d->state.ySpeed > 0)
        {
            d->state.ySpeed -= Y_FRICTION;

            if(d->state.ySpeed < 0)
            {
                d->state.ySpeed = 0;
            }
        }
        else
        {
            d->state.ySpeed += Y_FRICTION;

            if(d->state.ySpeed > 0)
            {
                d->state.ySpeed = 0;
            }
        }
    }

    d->state.x += d->state.xSpeed;
    d->state.y += d->state.ySpeed;

    setPos(d->state.x, d->state.y);
}

}
