#include "world.h"

//Qt includes
#include <QDebug>

//using namespace AirPlug;

namespace GameApplication
{

World::World() : view(&scene)
{
    main_player.setRect(0,0,100,100);
    main_player.setBrush(QBrush(Qt::red));

    main_player.setFlag(QGraphicsItem::ItemIsFocusable);
    main_player.setFocus();
    scene.addItem(&main_player);

    view.show();

    connect(timer, SIGNAL(timeout), this, SLOT(frameTimeout()));
    frame_timer.start(50);
}

World::~World()
{

}

}

void frameTimeout()
{
    //QDebug() << "oi";
}
