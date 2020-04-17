#include "application_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>

namespace AirPlug
{

ApplicationController::ApplicationController(const QString& appName, QObject* parent)
    : QObject(parent),
      m_communication(nullptr),
      m_optionParser(nullptr)
{
    setObjectName(appName);
}

ApplicationController::~ApplicationController()
{
    delete m_communication;
    delete m_optionParser;
}

void ApplicationController::init(const QCoreApplication& app)
{
    m_optionParser = new OptionParser(app);

    // Debug
    m_optionParser->showOption();

    m_communication = new CommunicationManager(m_optionParser->ident,
                                               m_optionParser->destination,
                                               Header::airHost,
                                               m_optionParser->headerMode,
                                               this);

    m_communication->setSafeMode(m_optionParser->safemode);

    m_communication->subscribeAir(m_optionParser->source);

    connect(m_communication, SIGNAL(signalMessageReceived(Header,Message)),
            this,            SLOT(slotReceiveMessage(Header,Message)));

    if (m_optionParser->remote)
    {
        m_communication->addUdpTransporter(m_optionParser->remoteHost,
                                           m_optionParser->remotePort,
                                           MessageTransporter::MultiCast);
    }
    else
    {
        m_communication->addStdTransporter();
    }
}

void ApplicationController::sendMessage(const Message& message, const QString& what, const QString& who, const QString& where)
{
    if (m_optionParser != nullptr && m_communication != nullptr)
    {
        if(m_optionParser->remote)
        {
            m_communication->send(message, what, who, where,
                                  CommunicationManager::ProtocolType::UDP,
                                  m_optionParser->save);
        }
        else
        {
            m_communication->send(message, what, who, where,
                                  CommunicationManager::ProtocolType::StandardIO,
                                  m_optionParser->save);
        }
    }

}

int ApplicationController::getPeriod()  const
{
    if (m_optionParser)
    {
        return m_optionParser->delay;
    }

    return 0;
}

bool ApplicationController::hasGUI() const
{
    if (m_optionParser)
    {
        return (! m_optionParser->nogui);
    }

    return false;
}

bool ApplicationController::isStarted() const
{
    if (m_optionParser)
    {
        return m_optionParser->start;
    }

    return false;
}

bool ApplicationController::isAuto() const
{
    if (m_optionParser)
    {
        return m_optionParser->autoSend;
    }

    return false;
}

Header::HeaderMode ApplicationController::headerMode() const
{
    if (m_optionParser)
    {
        return m_optionParser->headerMode;
    }

    return Header::HeaderMode::What;
}

}

