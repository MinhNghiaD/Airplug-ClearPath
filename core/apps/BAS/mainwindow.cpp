#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>
#include <QDebug>

using namespace Ui;

namespace BasApplication
{

MainWindow::MainWindow(BasController* controller, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    controller(controller)
{
    ui->setupUi(this);

    if (!controller)
    {
        qFatal("BAS Controller is null");
    }

    connect(controller, SIGNAL(signalSequenceChange(int)),
            this,       SLOT(slotUpdateSequence(int)));

    int period = controller->getPeriod();

    ui->frequencySpinBox->setSingleStep(500);
    ui->frequencySpinBox->setMaximum(10000);
    ui->frequencySpinBox->setMinimum(500);

    if (period > 0)
    {
        ui->frequencySpinBox->setValue(period);
    }
    else
    {
        ui->frequencySpinBox->setValue(500);
    }

    if (controller->isAuto())
    {
        ui->emissionStatus->setText(QLatin1String("[state : active]"));
    }
    else
    {
        ui->emissionStatus->setText(QLatin1String("[etat : desactive]"));
    }

    if (controller->isStarted())
    {
        ui->startButton->setText(QLatin1String("Stop"));
    }
    else
    {
        ui->startButton->setText(QLatin1String("Start"));
    }

    ui->messageReceived->setText(QLatin1String(""));
    ui->messageSend->setText(QLatin1String("-"));
    ui->nbSequence->setText("0");

    pause(!controller->isStarted());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pause(bool b)
{
    ui->autoBtn->setEnabled(!b);
    ui->sendBtn->setEnabled(!b);
    ui->messageSend->setReadOnly(b);
    ui->frequencySpinBox->setReadOnly(b);

    controller->pause(b);
}

const QString MainWindow::getSendMessage() const
{
    return ui->messageSend->text();
}

// TODO receive message
void MainWindow::slotShowReceivedMessage(const QString& message)
{
    if (controller->isStarted())
    {
        ui->messageReceived->setText(message);
    }
}

void MainWindow::slotUpdateSequence(int nbSequence)
{
    ui->nbSequence->setText(QString::number(nbSequence));
}

void MainWindow::on_autoBtn_clicked()
{
    ui->emissionStatus->setText(QLatin1String("[state : active]"));

    controller->slotActivateTimer(ui->frequencySpinBox->value());
}

void MainWindow::on_sendBtn_clicked()
{
    controller->slotSendMessage();
}

void MainWindow::on_messageSend_textChanged(const QString& msg)
{
    controller->setMessage(msg);
}

void MainWindow::on_frequencySpinBox_valueChanged(int period)
{
   controller->slotPeriodChanged(period);
}

void MainWindow::on_startButton_clicked()
{
    if (controller->isStarted())
    {
        ui->startButton->setText(QLatin1String("Start"));
        pause(true);
    }
    else
    {
        ui->startButton->setText(QLatin1String("Stop"));
        pause(false);
    }
}

}

