/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-24
 * Description : main application of the game
 *
 * 2020 by Lucca Rawlyk
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

//Qt includes
#include <QApplication>
#include <QThread>
#include <QDebug>

//local includes
#include "world.h"
#include "agent_controller.h"

using namespace ClearPathApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GAM");

    QThread* eventThread = new QThread();

    Board* board = new Board(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

    AgentController* controller = new AgentController(board);
    controller->moveToThread(eventThread);
    eventThread->start();
    controller->init(app);

    World* view = nullptr;

    if (controller->hasGUI())
    {
        qDebug() << "------------------------- Start with GUI -------------------------------";

        view = new World(app, board);
        view->show();
    }

    return app.exec();

    eventThread->quit();
    eventThread->wait();

    delete eventThread;
    delete controller;
    delete view;
    delete board;
}
