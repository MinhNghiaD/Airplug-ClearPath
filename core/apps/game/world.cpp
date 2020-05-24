#include "world.h"

//qt include
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QBrush>
#include <QTimer>
#include <QLinearGradient>
#include <QColor>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QDebug>

//std includes
#include <vector>
#include <cmath>
#include <random>
#include <QList>

//local include
#include "state.h"
#include "player.h"
#include "constants.h"
#include "bas_controller.h"

namespace GameApplication
{

class Q_DECL_HIDDEN World::Private
{
public:
    Private(QCoreApplication &app)
        : view(&scene)
    {
    }

    ~Private()
    {
    }

public:

    void moveAndUpdatePlayer(Player &player);
    void fixCollisions(Player &player);

    QGraphicsScene scene;
    QGraphicsView view;

    Player main_player;
    std::vector<std::shared_ptr<Player>> connected_player;

    QTimer frame_timer;
};

World::World(QCoreApplication &app)
    : d(new Private(app))
{
    //initializing main_player
    d->main_player.setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);

    QLinearGradient linearGrad(QPointF(0, 0), QPointF(PLAYER_SIZE, PLAYER_SIZE));
    linearGrad.setColorAt(0,    Qt::red);
    linearGrad.setColorAt(0.35, Qt::yellow);
    linearGrad.setColorAt(0.5,  Qt::green);
    linearGrad.setColorAt(0.65, Qt::cyan);
    linearGrad.setColorAt(1,    Qt::blue);

    d->main_player.setBrush(QBrush(linearGrad));
    d->main_player.setFlag(QGraphicsItem::ItemIsFocusable);
    d->main_player.setFocus();

    d->scene.addItem(&d->main_player);
    d->scene.setSceneRect(0, 0, VIEW_WIDTH*2, VIEW_HEIGHT*2);

    std::random_device seeder{};
    std::mt19937 twister{seeder()};
    std::uniform_int_distribution<> x_dis(0, VIEW_WIDTH-1-PLAYER_SIZE);
    std::uniform_int_distribution<> y_dis(0, VIEW_HEIGHT-1-PLAYER_SIZE);

    d->main_player.setPos(x_dis(twister), y_dis(twister));

    std::vector<QColor> color = {Qt::white,Qt::black,Qt::cyan,Qt::red,Qt::magenta,Qt::green,Qt::yellow,Qt::blue,Qt::gray};
    std::uniform_int_distribution<> color_dis(0, color.size()-1);

    // int n_connected_players = 5;
    //
    // for(int i=0; i<n_connected_players; i++)
    // {
    //     d->connected_player.push_back(std::make_shared<Player>());
    //     d->connected_player[i]->setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);
    //     d->connected_player[i]->setBrush(QBrush(color[color_dis(twister)]));
    //     d->scene.addItem(d->connected_player[i].get());
    //
    //     //to be changed
    //     QList<QGraphicsItem*> colliding_items;
    //     do
    //     {
    //         d->connected_player[i]->setPos(x_dis(twister), y_dis(twister));
    //         colliding_items = d->connected_player[i]->collidingItems();
    //     } while(colliding_items.size() != 0);
    // }

    connect(&d->frame_timer, SIGNAL(timeout()), this, SLOT(slotFrameTimeout()));
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
    d->main_player.incrementFrame();

    bool all_updated = false;
    while(!all_updated)
    {
        all_updated = true;

        for(auto it = d->connected_player.begin();
                 it != d->connected_player.end();
               ++it)
        {
            if((*it)->getFrame() != d->main_player.getFrame())
            {
                all_updated = false;
                break;
            }
        }
    }

    d->moveAndUpdatePlayer(d->main_player);
    for(auto it = d->connected_player.begin(); it != d->connected_player.end(); ++it)
        d->moveAndUpdatePlayer(**it);

    d->fixCollisions(d->main_player);
    for(auto it = d->connected_player.begin(); it != d->connected_player.end(); ++it)
        d->fixCollisions(**it);
}

void World::slotPlayerUpdate(int player_index, QString player_state)
{
    if(player_index == d->connected_player.size())
    {
        std::random_device seeder{};
        std::mt19937 twister{seeder()};
        std::vector<QColor> color = {Qt::white,Qt::black,Qt::cyan,Qt::red,Qt::magenta,Qt::green,Qt::yellow,Qt::blue,Qt::gray};
        std::uniform_int_distribution<> color_dis(0, color.size()-1);

        d->connected_player.push_back(std::make_shared<Player>());
        d->connected_player[player_index]->setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);
        d->connected_player[player_index]->setBrush(QBrush(color[color_dis(twister)]));
        d->scene.addItem(d->connected_player[player_index].get());
    }

    QJsonObject obj = QJsonDocument::fromJson(player_state.toUtf8()).object();
    State new_state;
    new_state.loadFromJson(obj);
    d->connected_player[player_index]->setState(new_state);
}

void World::Private::moveAndUpdatePlayer(Player &player)
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
       state.ySpeed != 0)
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

}
