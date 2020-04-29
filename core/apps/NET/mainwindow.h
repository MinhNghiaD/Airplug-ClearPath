/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-29
 * Description : main window of NET application
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

//local include
#include "net_controller.h"

namespace Ui
{
    class MainWindow;
}

namespace NetApplication
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(NetController* controller, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_snapshotButton_clicked();

private:
    Ui::MainWindow *ui;

    NetController* m_controller;
};

}
#endif // MAINWINDOW_H
