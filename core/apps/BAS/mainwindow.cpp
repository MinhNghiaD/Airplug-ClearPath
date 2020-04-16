#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSpinBox>

namespace BasApplication
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->messageReceived->setText(QLatin1String(""));
    ui->messageSend->setText(QLatin1String("-"));
    ui->emissionStatus->setText(QLatin1String("[etat : desactive]"));
    ui->frequencySpinBox->setSingleStep(500);
    ui->frequencySpinBox->setMaximum(10000);
    ui->frequencySpinBox->setMinimum(500);
    ui->frequencySpinBox->setValue(500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QString MainWindow::getSendMessage() const
{
    return ui->messageSend->text();
}

void MainWindow::slotShowReceivedMessage(const QString& message)
{
    ui->messageReceived->setText(message);
}


void MainWindow::on_autoBtn_clicked()
{
    ui->emissionStatus->setText(QLatin1String("[etat : active]"));

    emit signalStartAuto(ui->frequencySpinBox->value());
}

void MainWindow::on_sendBtn_clicked()
{
    emit signalSendMessage();
}

void MainWindow::on_messageSend_textChanged(const QString& msg)
{
    emit signalSendingMessageChanged(msg);
}

void MainWindow::on_frequencySpinBox_valueChanged(int period)
{
    emit signalPeriodChanged(period);
}

}
