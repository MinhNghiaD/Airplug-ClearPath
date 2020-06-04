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
#include "aclmessage.h"
#include "board.h"

using namespace AirPlug;

namespace ClearPathApplication
{

class AgentController: public ApplicationController
{
    Q_OBJECT
public:

    AgentController(Board* board, QObject* parent = nullptr);
    ~AgentController();

    // Initialization of program
    void init(const QCoreApplication& app) override;

    //void pause(bool b);

    QJsonObject captureLocalState() const override;

public:

    Q_SLOT void slotDoStep();
    Q_SLOT void slotSendMessage(ACLMessage& message);

private:

    void sendLocalSnapshot();

private:

    // main notification handler
    Q_SLOT void slotReceiveMessage(Header, Message) override;

private:

    class Private;
    Private* d;
};

}

#endif // AGENT_CONTROLLER_H
