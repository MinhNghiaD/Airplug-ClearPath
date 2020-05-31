#include "agent.h"

//qt includes
#include <QDebug>
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
        : state(State(siteID)),
          radius(radius),
          updated(false)
    {
    }

    ~Private()
    {
    }

public:

    State state;
    qreal radius;
    bool  updated;

    static QRgb color;
};

QRgb Agent::Private::color = 5;

Agent::Agent(const QString& siteID, qreal radius)
    : QGraphicsEllipseItem(0, 0, radius, radius),
      d(new Private(siteID, radius))
{
    //setFlag(QGraphicsItem::ItemIsFocusable, true);
    setBrush(QBrush( Qt::GlobalColor(Private::color) ));
    Private::color += 5;
}

Agent::~Agent()
{
    delete d;
}

void Agent::init()
{
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    // Init position of player randomly
    std::random_device seeder{};
    std::mt19937 twister{seeder()};
    std::uniform_int_distribution<> x(0, VIEW_WIDTH - 1 - PLAYER_SIZE);
    std::uniform_int_distribution<> y(0, VIEW_HEIGHT - 1 - PLAYER_SIZE);

    d->state.x = x(twister);
    d->state.y = y(twister);

    setPos(d->state.x, d->state.y);

    d->updated = true;
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
    d->updated = true;

    setPos(d->state.x, d->state.y);
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

    d->updated = true;
}

void Agent::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Agent is updated" << d->updated;

    if (d->updated)
    {
        // TODO sync
        emit signalStateChanged();
        d->updated = false;
    }

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

}
