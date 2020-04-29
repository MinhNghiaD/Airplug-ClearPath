/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-22
 * Description : main application of NET
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
#include "mainwindow.h"

using namespace NetApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("NET");

    NetController controller;

    controller.init(app);

    MainWindow* mainWindow = nullptr;

    if (controller.hasGUI())
    {
        qDebug() << "------------------------- Start with GUI -------------------------------";

        mainWindow = new MainWindow(&controller);
        mainWindow->setWindowTitle(QLatin1String("NET"));

        mainWindow->show();
    }

    return app.exec();
}
