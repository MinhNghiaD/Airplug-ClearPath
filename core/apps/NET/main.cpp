/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : main application of BAS
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

//Qt includes
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>

//local includes
#include "net_controller.h"

using namespace NetApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("NET");

    NetController controller;

    controller.init(app);

    return app.exec();
}
