/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-17
 * Description : controller of BAS application
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

namespace AirPlug
{

class ApplicationController: public QObject
{
    Q_OBJECT
public:

    ApplicationController(QObject* parent = nullptr);
    ~ApplicationController();

    void parseOptions(const QCoreApplication& app);

    int  getPeriod() const;

    bool hasGUI()    const;

    bool isStarted() const;
    void pause(bool b);

    bool isAuto()    const;

    void setMessage(const QString& msg);

    Header::HeaderMode headerMode() const;

public:

    Q_SLOT void slotActivateTimer(int period);
    Q_SLOT void slotDeactivateTimer();
    Q_SLOT void slotPeriodChanged(int period);
    Q_SLOT void slotSendMessage();

signals:

    Q_SIGNAL void signalSequenceChange(int);

    Q_SIGNAL void signalMessageReceived(Header, Message);

private:

    class Private;
    Private* d;
};

}

#endif // APPLICATION_CONTROLLER_H
