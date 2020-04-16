/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : main window of BAS application
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

//local include
#include "communication_manager.h"

namespace Ui
{
    class MainWindow;
}

namespace BasApplication
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    const QString getSendMessage() const;


public slots:

    void slotShowReceivedMessage(const QString& message);

private slots:

    void on_autoBtn_clicked();

    void on_sendBtn_clicked();

    void on_messageSend_textChanged(const QString &arg1);

    void on_frequencySpinBox_valueChanged(int arg1);

signals:

    void signalStartAuto(int period);

    void signalSendMessage();

    void signalSendingMessageChanged(const QString& message);

    void signalPeriodChanged(int period);

private:
    Ui::MainWindow *ui;
};

}
#endif // MAINWINDOW_H
