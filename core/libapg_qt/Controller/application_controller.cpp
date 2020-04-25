#include "application_controller.h"

//Qt includes

#include <QUuid>
#include <QDebug>

namespace AirPlug
{

ApplicationController::ApplicationController(const QString& appName, QObject* parent)
    : QObject(parent),
      m_communication(nullptr),
      m_clock(new VectorClock(generatedSiteID()))
{
    setObjectName(appName);
}

ApplicationController::~ApplicationController()
{
    delete m_communication;
    delete m_clock;
}

void ApplicationController::init(const QCoreApplication& app)
{
    // parse application options
    m_optionParser.parseOptions(app);
    m_optionParser.showOption();

    // setup transport layer
    m_communication = new CommunicationManager(m_optionParser.ident,
                                               m_optionParser.destination,
                                               Header::airHost,
                                               m_optionParser.headerMode,
                                               this);

    m_communication->setSafeMode(m_optionParser.safemode);

    m_communication->subscribeAir(m_optionParser.source);

    connect(m_communication, SIGNAL(signalMessageReceived(Header,Message)),
            this,            SLOT(slotReceiveMessage(Header,Message)));

    if (m_optionParser.remote)
    {
        m_communication->addUdpTransporter(m_optionParser.remoteHost,
                                           m_optionParser.remotePort,
                                           MessageTransporter::MultiCast);
    }
    else
    {
        m_communication->addStdTransporter();
    }
}

void ApplicationController::sendMessage(const Message& message, const QString& what, const QString& who, const QString& where)
{
    if (m_communication != nullptr)
    {
        if(m_optionParser.remote)
        {
            m_communication->send(message, what, who, where,
                                  CommunicationManager::ProtocolType::UDP,
                                  m_optionParser.save);
        }
        else
        {
            m_communication->send(message, what, who, where,
                                  CommunicationManager::ProtocolType::StandardIO,
                                  m_optionParser.save);
        }
    }

}

int ApplicationController::getPeriod()  const
{
    return m_optionParser.delay;
}

bool ApplicationController::hasGUI() const
{
    return (! m_optionParser.nogui);
}

bool ApplicationController::isStarted() const
{
    return m_optionParser.start;
}

bool ApplicationController::isAuto() const
{
    return m_optionParser.autoSend;
}

Header::HeaderMode ApplicationController::headerMode() const
{
    return m_optionParser.headerMode;
}

QString ApplicationController::generatedSiteID()
{
    // TODO: use more effective method to generate UUID
    // Generate random UUID
    return QUuid::createUuid().toString();
}

QJsonObject ApplicationController::captureLocalState() const
{
    QJsonObject localState = m_clock->convertToJson();
    localState[QLatin1String("options")] = m_optionParser.convertToJson();

    return localState;
}

}

