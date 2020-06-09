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
#include <QGraphicsView>
#include <QThread>
#include <QDebug>

//local includes
#include "agent_controller.h"

using namespace ClearPathApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("GAM");

    AgentController* controller = new AgentController();
    controller->init(app);

    Board* board = controller->getBoard();
    QGraphicsView* view = nullptr;


    if (board)
    {
        view = new QGraphicsView(board);
        view->show();
    }

    return app.exec();
    delete view;
    delete controller;
/*
    QThread* eventThread = new QThread();

    controller->moveToThread(eventThread);
    eventThread->start();

    eventThread->quit();
    eventThread->wait();

    delete eventThread;
*/
}
