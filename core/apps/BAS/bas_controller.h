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

namespace BasApplication
{

class BasController: public QObject
{
    Q_OBJECT
public:

    BasController(QObject* parent = nullptr);
    ~BasController();

    void setTimer(int period);
    int  getPeriod() const;

    void parseOptions(const QCoreApplication& app);

public:

    Q_SLOT void slotSendingMessageChanged(const QString& msg);
    Q_SLOT void slotActivateTimer(int period);
    Q_SLOT void slotDeactivateTimer();
    Q_SLOT void slotPeriodChanged(int period);
    Q_SLOT void slotSendMessage() const;

private:

    class Private;
    Private* d;
};

}
#endif // BAS_CONTROLLER_H
