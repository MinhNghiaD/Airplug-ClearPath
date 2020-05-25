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
#include <QDebug>

//local includes
#include "world.h"
#include "agent_controller.h"

using namespace GameApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GAM");

    AgentController controller;

    controller.init(app);

    World* world = nullptr;

    if (controller.hasGUI())
    {
        qDebug() << "------------------------- Start with GUI -------------------------------";

        world = new World(app, controller.getBoard());

        world->show();
    }

    return app.exec();

    delete world;
}
