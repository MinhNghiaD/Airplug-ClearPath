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
#include "vector_clock.h"

namespace AirPlug
{

class ApplicationController: public QObject
{
    Q_OBJECT
public:

    ApplicationController(const QString& appName, QObject* parent = nullptr);
    ~ApplicationController();

    /**
     * @brief init: Initialization of controller
     * @param app
     */
    virtual void init(const QCoreApplication& app);

    int  getPeriod() const;

    bool hasGUI()    const;

    bool isStarted() const;

    bool isAuto()    const;

    Header::HeaderMode headerMode() const;

    QString siteID() const;

    /**
     * @brief sendMessage: send message to Transporter interface
     * @param message
     * @param what
     * @param who
     * @param where
     */
    virtual void sendMessage(const Message& message, const QString& what, const QString& who, const QString& where);

public:

    /**
     * @brief generatedSiteID : generate random siteID by random UUID generator, therefore the probability of having 2 identical IDs is really low
     * @return
     */
    static QString generatedSiteID();

    /**
     * @brief captureLocalState : save local state of the application to a Json object for snapshot.
     * The general state will contains siteID, local clock and application local variable
     * @return
     */
    virtual QJsonObject captureLocalState() const;

public:

    /**
     * slotReceiveMessage: main notification handler
     *
     */
    Q_SLOT virtual void slotReceiveMessage(Header, Message) = 0;

protected:

    CommunicationManager* m_communication;

    OptionParser          m_optionParser;

    VectorClock*          m_clock;
};

}

#endif // APPLICATION_CONTROLLER_H
