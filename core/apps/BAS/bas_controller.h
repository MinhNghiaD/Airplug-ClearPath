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
#include "option_parser.h"
#include "communication_manager.h"

using namespace AirPlug;

namespace BasApplication
{

class BasController: public QObject
{
    Q_OBJECT
public:

    BasController(QObject* parent = nullptr);
    ~BasController();

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
#endif // BAS_CONTROLLER_H
