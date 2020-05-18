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
    m_controller(controller)
{
    ui->setupUi(this);

    if (!m_controller)
    {
        qFatal("BAS Controller is null");
    }

    connect(m_controller, SIGNAL(signalSequenceChange(int)),
            this,         SLOT(slotUpdateSequence(int)));

    if (!m_controller->siteID().isEmpty())
    {
        setWindowTitle(m_controller->siteID());
    }
    else
    {
        setWindowTitle(QString("BAS %1").arg(QCoreApplication::applicationPid()));
    }

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

    if (m_controller->isAuto())
    {
        ui->emissionStatus->setText(QLatin1String("[state : active]"));
    }
    else
    {
        ui->emissionStatus->setText(QLatin1String("[etat : desactive]"));
    }

    if (m_controller->isStarted())
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

    connect(m_controller, &BasController::signalMessageReceived,
            this,         &MainWindow::slotShowReceivedMessage);

    pause(!m_controller->isStarted());
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

    m_controller->pause(b);
}

const QString MainWindow::getSendMessage() const
{
    return ui->messageSend->text();
}

// TODO receive message
void MainWindow::slotShowReceivedMessage(Header header, Message message)
{
    if (m_controller->isStarted())
    {
        ACLMessage* aclMessage = static_cast<ACLMessage*>(&message);
        QString sequence       = aclMessage->getContent()[QLatin1String("payload")].toString();
        QString content        = aclMessage->getSender()+ " > Current sequence : " + sequence;

        ui->messageReceived->setText(content);
    }
}

void MainWindow::slotUpdateSequence(int nbSequence)
{
    ui->nbSequence->setText(QString::number(nbSequence));
}

void MainWindow::on_autoBtn_clicked()
{
    ui->emissionStatus->setText(QLatin1String("[state : active]"));

    m_controller->slotActivateTimer(ui->frequencySpinBox->value());
}

void MainWindow::on_sendBtn_clicked()
{
    m_controller->slotSendMessage();
}

void MainWindow::on_messageSend_textChanged(const QString& msg)
{
    m_controller->setMessage(msg);
}

void MainWindow::on_frequencySpinBox_valueChanged(int period)
{
   m_controller->slotPeriodChanged(period);
}

void MainWindow::on_startButton_clicked()
{
    if (m_controller->isStarted())
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
