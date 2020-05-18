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
#include "mainwindow.h"

#include "bas_controller.h"

using namespace BasApplication;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("BAS");

    BasController controller;

    controller.init(app);

    MainWindow* mainWindow = nullptr;

    if (controller.hasGUI())
    {
        qDebug() << "------------------------- Start with GUI -------------------------------";

        mainWindow = new MainWindow(&controller);

        mainWindow->show();
    }

    return app.exec();
}
