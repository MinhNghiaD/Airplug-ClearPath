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

    // -------------------------------------------- Parse arguments -----------------------------------------------------------------------


    // -------------------------------------------- Parse arguments -----------------------------------------------------------------------




    //MainWindow mainWindow;
    //mainWindow.setWindowTitle(QString("Light %1").arg(QCoreApplication::applicationPid()));

    //BasController controller;
/*
    //connect signals slot between main window and sender
    QObject::connect(&mainWindow, SIGNAL(signalStartAuto(int)),
                     &sender,     SLOT(slotActivateTimer(int)));

    QObject::connect(&mainWindow, SIGNAL(signalSendMessage()),
                     &sender,     SLOT(slotSendMessage()));

    QObject::connect(&mainWindow, SIGNAL(signalSendingMessageChanged(QString)),
                     &sender,     SLOT(slotSendingMessageChanged(QString)));

    QObject::connect(&mainWindow, SIGNAL(signalPeriodChanged(int)),
                     &sender,     SLOT(slotPeriodChanged(int)));

    //connect signals slots between main window and receiver
    QObject::connect(&receiver,   SIGNAL(signalMessageReceived(const QString&)),
                     &mainWindow, SLOT(slotShowReceivedMessage(const QString&)));
*/
    //mainWindow.show();

    return app.exec();
}
