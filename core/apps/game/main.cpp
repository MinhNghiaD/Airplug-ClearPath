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

//std includes
//#include <memory>

//local includes
#include "world.h"

using namespace GameApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("game");

    World world(app);

    return app.exec();
}
