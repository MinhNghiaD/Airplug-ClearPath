/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-24
 * Description : main application of the game
 *
 * 2020 by Lucca Rawlyk
 *
 * ============================================================ */

//Qt includes
#include <QApplication>

//local includes
#include "world.h"

using namespace GameApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("game");

    Board* board = new Board(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

    Agent* agent1 = new Agent(QLatin1String("bas"), AGENT_RADIUS);
    board->addAgent(QLatin1String("bas"), agent1);

    World* world = new World(app, board);

    world->show();

    return app.exec();

    delete world;
    delete board;
    delete agent1;
}
