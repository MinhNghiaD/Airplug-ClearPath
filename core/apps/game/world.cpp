#include "world.h"

//Qt includes
#include <QDebug>

//using namespace AirPlug;

namespace GameApplication
{

World::World() : view(&scene)
{
    //initializing main_player
    main_player.setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);

    QLinearGradient linear_grad(QPointF(0, 0), QPointF(PLAYER_SIZE, PLAYER_SIZE));
    linear_grad.setColorAt(0, Qt::red);
    linear_grad.setColorAt(0.35, Qt::yellow);
    linear_grad.setColorAt(0.5, Qt::green);
    linear_grad.setColorAt(0.65, Qt::cyan);
    linear_grad.setColorAt(1, Qt::blue);
    main_player.setBrush(QBrush(linear_grad));

    main_player.setFlag(QGraphicsItem::ItemIsFocusable);
    main_player.setFocus();

    scene.addItem(&main_player);

    scene.setSceneRect(0, 0, VIEW_WIDTH*2, VIEW_HEIGHT*2);

    std::random_device seeder{};
    std::mt19937 twister{seeder()};
    std::uniform_int_distribution<> x_dis(0, VIEW_WIDTH-1-PLAYER_SIZE);
    std::uniform_int_distribution<> y_dis(0, VIEW_HEIGHT-1-PLAYER_SIZE);

    main_player.setPos(x_dis(twister), y_dis(twister));

    std::vector<QColor> color = {Qt::white,Qt::black,Qt::cyan,Qt::red,Qt::magenta,Qt::green,Qt::yellow,Qt::blue,Qt::gray};
    std::uniform_int_distribution<> color_dis(0, color.size()-1);

    //TODO
    //Establish the communication
    //Receive other players data

    int n_connected_players = 5;

    for(int i=0; i<n_connected_players; i++)
    {
        connected_player.push_back(std::make_shared<Player>());

        connected_player[i]->setRect(0,0,PLAYER_SIZE,PLAYER_SIZE);

        connected_player[i]->setBrush(QBrush(color[color_dis(twister)]));

        scene.addItem(connected_player[i].get());

        //to be changed
        QList<QGraphicsItem*> colliding_items;
        do
        {
            connected_player[i]->setPos(x_dis(twister), y_dis(twister));
            colliding_items = connected_player[i]->collidingItems();
        } while(colliding_items.size() != 0);
    }

    connect(&frame_timer, SIGNAL(timeout()), this, SLOT(frameTimeout()));
    frame_timer.start(FRAME_PERIOD_MS);

    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setFixedSize(VIEW_WIDTH, VIEW_HEIGHT);
    view.show();
}

World::~World()
{

}

void World::frameTimeout(void)
{
    //TODO
    //update connected players

    moveAndUpdatePlayer(main_player);
    // for(auto it = connected_player.begin(); it != connected_player.end(); ++it)
    //     moveAndUpdatePlayer(**it);

    fixCollisions(main_player);
    for(auto it = connected_player.begin(); it != connected_player.end(); ++it)
        fixCollisions(**it);
}

void World::moveAndUpdatePlayer(Player &player)
{
    State state = player.getState();

    //apply speed modifications from controls
    if(state.left == true)
        state.x_speed -= X_CONTROL_ACCELERATION;
    else if(state.right == true)
        state.x_speed += X_CONTROL_ACCELERATION;

    if(abs(state.x_speed) > X_SPEED_LIMIT)
        state.x_speed = (state.x_speed > 0) ? X_SPEED_LIMIT : -1*X_SPEED_LIMIT;

    if(state.up == true)
        state.y_speed -= Y_CONTROL_ACCELERATION;
    else if(state.down == true)
        state.y_speed += Y_CONTROL_ACCELERATION;

    if(abs(state.y_speed) > Y_SPEED_LIMIT)
        state.y_speed = (state.y_speed >= 0) ? Y_SPEED_LIMIT : -1*Y_SPEED_LIMIT;

    //apply speed reduction from friction
    if(state.left == false && state.right == false && state.x_speed != 0)
    {
        if(state.x_speed > 0)
        {
            state.x_speed -= X_FRICTION;
            if(state.x_speed < 0)
                state.x_speed = 0;
        }
        else
        {
            state.x_speed += X_FRICTION;
            if(state.x_speed > 0)
                state.x_speed = 0;
        }
    }

    if(state.up == false && state.down == false && state.y_speed != 0)
    {
        if(state.y_speed > 0)
        {
            state.y_speed -= Y_FRICTION;
            if(state.y_speed < 0)
                state.y_speed = 0;
        }
        else
        {
            state.y_speed += Y_FRICTION;
            if(state.y_speed > 0)
                state.y_speed = 0;
        }
    }

    //write changes
    player.setSpeed(state.x_speed, state.y_speed);
    player.setPos(state.x + state.x_speed, state.y + state.y_speed);
}

void World::fixCollisions(Player &player)
{
    //lazy fixing, might need to be improved
    QList<QGraphicsItem*> colliding_items = player.collidingItems();
    while(colliding_items.size() != 0)
    {
        auto col = static_cast<Player*>(colliding_items[0]);
        State p_state = player.getState();
        if(p_state.x_speed > 0)
        {
            if(p_state.y_speed > 0)
            {
                player.setPos(player.x()-1, player.y()-1);
                col->setPos(col->x()+1, col->y()+1);
            }
            else
            {
                player.setPos(player.x()-1, player.y()+1);
                col->setPos(col->x()+1, col->y()-1);
            }
        }
        else
        {
            if(p_state.y_speed > 0)
            {
                player.setPos(player.x()+1, player.y()-1);
                col->setPos(col->x()-1, col->y()+1);
            }
            else
            {
                player.setPos(player.x()+1, player.y()+1);
                col->setPos(col->x()-1, col->y()-1);
            }
        }
        player.setSpeed(0,0);
        col->setSpeed(0,0);
        colliding_items = player.collidingItems();
    }
}

}
