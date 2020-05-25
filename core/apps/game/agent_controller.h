
/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-25
 * Description : controller of Agent
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef AGENT_CONTROLLER_H
#define AGENT_CONTROLLER_H

// Qt include
#include <QObject>

// Local include
#include "application_controller.h"

#include "ricart_lock.h"
#include "board.h"

using namespace AirPlug;

namespace GameApplication
{

class AgentController: public ApplicationController
{
    Q_OBJECT
public:

    AgentController(QObject* parent = nullptr);
    ~AgentController();

    // Initialization of program
    void init(const QCoreApplication& app) override;

    //void pause(bool b);

    QJsonObject captureLocalState() const override;

    Board* getBoard() const;

public:

    //Q_SLOT void slotActivateTimer(int period);
    //Q_SLOT void slotDeactivateTimer();
    //Q_SLOT void slotPeriodChanged(int period);
    Q_SLOT void slotSendMessage();


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

#endif // AGENT_CONTROLLER_H
