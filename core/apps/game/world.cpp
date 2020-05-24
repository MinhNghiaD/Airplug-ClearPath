#include "world.h"

//qt include

#include <QBrush>
#include <QTimer>
#include <QLinearGradient>
#include <QColor>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QDebug>
#include <QVector>

//std includes
#include <cmath>

#include <QList>

//local include
#include "state.h"

#include "constants.h"
#include "bas_controller.h"

// TODO: change player shape to round
// TODO: reorganize the code
// TODO: get nb of player from Bas Controller and init board
// TODO: update for each move
// TODO: sync
// TODO: Mode Auto
// TODO: clearpath

namespace GameApplication
{

class Q_DECL_HIDDEN World::Private
{
public:
    Private()
        : board(nullptr)
    {
    }

    ~Private()
    {
    }

public:

    //void moveAndUpdatePlayer(Player &player);
    //void fixCollisions(Player &player);

    //QGraphicsView* view;
    Board* board;

    QTimer frame_timer;
};

World::World(const QCoreApplication &app, Board* board)
    : QGraphicsView(board),
      d(new Private())
{
    d->board = board;
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(VIEW_WIDTH, VIEW_HEIGHT);
}

World::~World()
{
    delete d;
}

/*
World::World(const QCoreApplication &app)
    : d(new Private(app))
{

    //initializing mainPlayer
    d->mainPlayer.setRect(0, 0, PLAYER_SIZE, PLAYER_SIZE);

    QLinearGradient linearGrad(QPointF(0, 0), QPointF(PLAYER_SIZE, PLAYER_SIZE));
    linearGrad.setColorAt(0,    Qt::red);
    linearGrad.setColorAt(0.35, Qt::yellow);
    linearGrad.setColorAt(0.5,  Qt::green);
    linearGrad.setColorAt(0.65, Qt::cyan);
    linearGrad.setColorAt(1,    Qt::blue);

    d->mainPlayer.setBrush(QBrush(linearGrad));
    d->mainPlayer.setFlag(QGraphicsItem::ItemIsFocusable);
    d->mainPlayer.setFocus();

    d->scene.addItem(&d->mainPlayer);
    d->scene.setSceneRect(0, 0, VIEW_WIDTH*2, VIEW_HEIGHT*2);

    // Init position of player randomly
    std::random_device seeder{};
    std::mt19937 twister{seeder()};
    std::uniform_int_distribution<> x_dis(0, VIEW_WIDTH - 1 - PLAYER_SIZE);
    std::uniform_int_distribution<> y_dis(0, VIEW_HEIGHT - 1 - PLAYER_SIZE);

    d->mainPlayer.setPos(x_dis(twister), y_dis(twister));

//    std::vector<QColor> color = {Qt::white,Qt::black,Qt::cyan,Qt::red,Qt::magenta,Qt::green,Qt::yellow,Qt::blue,Qt::gray};
//    std::uniform_int_distribution<> color_dis(0, color.size()-1);

//     int n_otherPlayerss = 5;

//     for(int i=0; i<n_otherPlayerss; i++)
//     {
//         d->otherPlayers.push_back(std::make_shared<Player>());
//         d->otherPlayers[i]->setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);
//         d->otherPlayers[i]->setBrush(QBrush(color[color_dis(twister)]));
//         d->scene.addItem(d->otherPlayers[i].get());

//         //to be changed
//         QList<QGraphicsItem*> colliding_items;
//         do
//         {
//             d->otherPlayers[i]->setPos(x_dis(twister), y_dis(twister));
//             colliding_items = d->otherPlayers[i]->collidingItems();
//         } while(colliding_items.size() != 0);
//     }

    connect(&d->frame_timer, SIGNAL(timeout()),
            this,            SLOT(slotFrameTimeout()));

    d->frame_timer.start(FRAME_PERIOD_MS);

    d->view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view.setFixedSize(VIEW_WIDTH, VIEW_HEIGHT);

    d->view.show();
}

World::~World()
{
    delete d;
}

void World::slotFrameTimeout(void)
{
    d->mainPlayer.incrementFrame();

    bool allUpdated = false;

    while(!allUpdated)
    {
        allUpdated = true;

        for(QVector<Player>::const_iterator it  = d->otherPlayers.cbegin();
                                            it != d->otherPlayers.cend();
                                          ++it)
        {
            if(it->getFrame() != d->mainPlayer.getFrame())
            {
                allUpdated = false;

                break;
            }
        }
    }

    d->moveAndUpdatePlayer(d->mainPlayer);

    for(QVector<Player>::const_iterator it  = d->otherPlayers.cbegin();
                                        it != d->otherPlayers.cend();
                                      ++it)
    {
        d->moveAndUpdatePlayer(*it);
    }

    d->fixCollisions(d->mainPlayer);

    for(QVector<Player>::iterator it  = d->otherPlayers.begin();
                                  it != d->otherPlayers.end();
                                ++it)
    {
        d->fixCollisions(*it);
    }
}

void World::slotPlayerUpdate(int player_index, QString player_state)
{

    if(player_index == d->otherPlayers.size())
    {
        std::random_device seeder{};
        std::mt19937 twister{seeder()};
        std::vector<QColor> color = {Qt::white, Qt::black, Qt::cyan, Qt::red, Qt::magenta, Qt::green, Qt::yellow, Qt::blue, Qt::gray};
        std::uniform_int_distribution<> color_dis(0, color.size()-1);

        d->otherPlayers.push_back(std::make_shared<Player>());
        d->otherPlayers[player_index].setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);
        d->otherPlayers[player_index].setBrush(QBrush(color[color_dis(twister)]));

        d->scene.addItem(d->otherPlayers[player_index].get());
    }

    QJsonObject obj = QJsonDocument::fromJson(player_state.toUtf8()).object();
    State new_state;
    new_state.loadFromJson(obj);

    d->otherPlayers[player_index].setState(new_state);

}

void World::Private::moveAndUpdatePlayer(Player& player)
{
    State state = player.getState();

    //apply speed modifications from controls
    if(state.left == true)
    {
        state.xSpeed -= X_CONTROL_ACCELERATION;
    }
    else if(state.right == true)
    {
        state.xSpeed += X_CONTROL_ACCELERATION;
    }

    if(abs(state.xSpeed) > X_SPEED_LIMIT)
    {
        state.xSpeed = (state.xSpeed > 0) ? X_SPEED_LIMIT : -1*X_SPEED_LIMIT;
    }

    if(state.up == true)
    {
        state.ySpeed -= Y_CONTROL_ACCELERATION;
    }
    else if(state.down == true)
    {
        state.ySpeed += Y_CONTROL_ACCELERATION;
    }

    if(abs(state.ySpeed) > Y_SPEED_LIMIT)
    {
        state.ySpeed = (state.ySpeed >= 0) ? Y_SPEED_LIMIT : -1*Y_SPEED_LIMIT;
    }

    //apply speed reduction from friction
    if(state.left == false && state.right == false && state.xSpeed != 0)
    {
        if(state.xSpeed > 0)
        {
            state.xSpeed -= X_FRICTION;
            if(state.xSpeed < 0)
            {
                state.xSpeed = 0;
            }
        }
        else
        {
            state.xSpeed += X_FRICTION;
            if(state.xSpeed > 0)
            {
                state.xSpeed = 0;
            }
        }
    }

    if(state.up      == false &&
       state.down    == false &&
       state.ySpeed  != 0)
    {
        if(state.ySpeed > 0)
        {
            state.ySpeed -= Y_FRICTION;
            if(state.ySpeed < 0)
            {
                state.ySpeed = 0;
            }
        }
        else
        {
            state.ySpeed += Y_FRICTION;
            if(state.ySpeed > 0)
            {
                state.ySpeed = 0;
            }
        }
    }

    //write changes
    player.setSpeed(state.xSpeed, state.ySpeed);
    player.setPos(state.x + state.xSpeed, state.y + state.ySpeed);

    // TODO: broadcast self state for other
}

void World::Private::fixCollisions(Player &player)
{
    //TODO lazy fixing, might need to be improved
    QList<QGraphicsItem*> colliding_items = player.collidingItems();
    bool collided = false;

    while(colliding_items.size() != 0)
    {
        collided = true;
        auto col = static_cast<Player*>(colliding_items[0]);
        State p_state = player.getState();

        if(p_state.xSpeed > 0)
        {
            if(p_state.ySpeed > 0)
            {
                player.setPos(player.x()-1, player.y()-1);
                col->setPos(col->x()+1, col->y()+1);
            }
            else if(p_state.ySpeed < 0)
            {
                player.setPos(player.x()-1, player.y()+1);
                col->setPos(col->x()+1, col->y()-1);
            }
            else
            {
                player.setPos(player.x()-1, player.y());
                col->setPos(col->x()+1, col->y());
            }
        }
        else if(p_state.xSpeed < 0)
        {
            if(p_state.ySpeed > 0)
            {
                player.setPos(player.x()+1, player.y()-1);
                col->setPos(col->x()-1, col->y()+1);
            }
            else if(p_state.ySpeed < 0)
            {
                player.setPos(player.x()+1, player.y()+1);
                col->setPos(col->x()-1, col->y()-1);
            }
            else
            {
                player.setPos(player.x()+1, player.y());
                col->setPos(col->x()-1, col->y());
            }
        }
        else
        {
            if(p_state.ySpeed > 0)
            {
                player.setPos(player.x(), player.y()-1);
                col->setPos(col->x(), col->y()+1);
            }
            else
            {
                player.setPos(player.x(), player.y()+1);
                col->setPos(col->x(), col->y()-1);
            }
        }

        col->setSpeed(0,0);
        colliding_items = player.collidingItems();
    }

    if(collided)
    {
        player.setSpeed(0,0);
    }
}
*/
}
