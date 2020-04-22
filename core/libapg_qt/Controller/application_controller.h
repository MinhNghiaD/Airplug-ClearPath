/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-17
 * Description : genetic application controller
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

// Qt include
#include <QObject>

// Local include
#include "option_parser.h"
#include "communication_manager.h"
#include "laiyang_snapshot.h"

namespace AirPlug
{

class ApplicationController: public QObject
{
    Q_OBJECT
public:

    ApplicationController(const QString& appName, QObject* parent = nullptr);
    ~ApplicationController();

    // Initialization
    virtual void init(const QCoreApplication& app);

    int  getPeriod() const;

    bool hasGUI()    const;

    bool isStarted() const;

    bool isAuto()    const;

    Header::HeaderMode headerMode() const;

    virtual void sendMessage(const Message& message, const QString& what, const QString& who, const QString& where);

public:

    // main notification handler
    Q_SLOT virtual void slotReceiveMessage(Header, Message) = 0;

protected:

    CommunicationManager* m_communication;

    OptionParser          m_optionParser;

    LaiYangSnapshot       m_snapshot;
};

}

#endif // APPLICATION_CONTROLLER_H
