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

    Board* board = nullptr;
    QGraphicsView* view = nullptr;


    if (controller->hasGUI())
    {
        board = new Board();
        controller->setBoard(board);
        view = new QGraphicsView(board);
        view->setWindowTitle(controller->siteID());
        view->show();
    }


    return app.exec();
    delete controller;
    delete view;
    delete board;
/*
    QThread* eventThread = new QThread();

    controller->moveToThread(eventThread);
    eventThread->start();

    eventThread->quit();
    eventThread->wait();

    delete eventThread;
*/
}
