/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-14
 * Description : controller of BAS application
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef BAS_CONTROLLER_H
#define BAS_CONTROLLER_H

// Qt include
#include <QObject>

// Local include
#include "application_controller.h"

#include "ricart_lock.h"

using namespace AirPlug;

namespace BasApplication
{

class BasController: public ApplicationController
{
    Q_OBJECT
public:

    BasController(QObject* parent = nullptr);
    ~BasController();

    // Initialization of program
    void init(const QCoreApplication& app) override;

    void pause(bool b);

    void setMessage(const QString& msg);

    QJsonObject captureLocalState() const override;

public:

    Q_SLOT void slotActivateTimer(int period);
    Q_SLOT void slotDeactivateTimer();
    Q_SLOT void slotPeriodChanged(int period);
    Q_SLOT void slotSendMessage();

signals:

    Q_SIGNAL void signalSequenceChange(int);

    Q_SIGNAL void signalMessageReceived(Header, Message);

private:

    void receiveMutexRequest(const ACLMessage& request) const;
    void sendLocalSnapshot();

private:

    // main notification handler
    Q_SLOT void slotReceiveMessage(Header, Message) override;
    Q_SLOT void slotForwardMutex(const ACLMessage&);

    Q_SLOT void slotEnterCriticalSection();

private:

    class Private;
    Private* d;
};

}
#endif // BAS_CONTROLLER_H
