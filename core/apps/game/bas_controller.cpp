#include "bas_controller.h"

//Qt includes
#include <QCoreApplication>
#include <QTimer>
#include <QMap>
#include <QPair>
#include <QDebug>

//local includes
#include "constants.h"

using namespace AirPlug;

namespace GameApplication
{

class Q_DECL_HIDDEN BasController::Private
{
public:

    Private()
    {

    }

    ~Private()
    {

    }

public:

    bool handshake_timeout = false;
    bool accepting_new_connections = true;
    QMap<int,QPair<int,QString>> connection_state;
    int established_connections = 0;
};



BasController::BasController(QCoreApplication &app, QObject* parent)
    : ApplicationController(QString("game"), parent),
      d(std::make_unique<Private>())
{
    ApplicationController::init(app);
}

BasController::~BasController()
{

}

void BasController::establishConnections(QString mp_state)
{
    Message message;
    message.addContent(QString("action"), QString("SYN"));
    message.addContent(QString("id"), QString::number(QCoreApplication::applicationPid()));
    message.addContent(QString("state"), mp_state);
    sendMessage(message, QString(), QString(), QString());

    bool all_acks_received = false;
    QTimer::singleShot(HANDSHAKE_TIMEOUT, this, SLOT(handshakeTimeout()));
    while(!d->handshake_timeout || !all_acks_received)
    {
        all_acks_received = true;
        for(auto key : d->connection_state.keys())
        {
            if(d->connection_state[key].second != QString("ACK"))
            {
                all_acks_received = false;
                break;
            }
        }
    }
    d->accepting_new_connections = false;
}

void BasController::handshakeTimeout(void)
{
    d->handshake_timeout = true;
}

void BasController::sendPlayerUpdate(QString mp_state)
{
    Message message;
    message.addContent(QString("action"), QString("UPD"));
    message.addContent(QString("id"), QString::number(QCoreApplication::applicationPid()));
    message.addContent(QString("state"), mp_state);
    sendMessage(message, QString(), QString(), QString());
}

void BasController::slotReceiveMessage(Header header, Message message)
{
    QHash<QString, QString> contents = message.getContents();
    if(contents.contains(QString("action")))
    {
        if(contents[QString("action")] == "SYN")
        {
            if(d->accepting_new_connections)
            {
                if(contents.contains(QString("id")) && contents.contains(QString("state")))
                {
                    int id = contents[QString("id")].toInt();
                    if(!d->connection_state.contains(id))
                    {
                        d->connection_state[id] = QPair<int, QString>(d->established_connections, contents[QString("action")]);
                        d->established_connections++;
                        emit updatePlayer(d->connection_state[id].first, contents[QString("state")]);

                        Message ack_message;
                        ack_message.addContent(QString("action"), QString("ACK"));
                        ack_message.addContent(QString("id"), QString::number(QCoreApplication::applicationPid()));
                        sendMessage(ack_message, QString(), QString(), QString());
                    }
                }
            }
        }
        else if(contents[QString("action")] == "ACK")
        {
            if(d->accepting_new_connections)
            {
                if(contents.contains(QString("id")))
                {
                    int id = contents[QString("id")].toInt();
                    if(d->connection_state.contains(id))
                        d->connection_state[id].second = "ACK";
                }
            }
        }
        else if(contents[QString("action")] == "UPD")
        {
            if(contents.contains(QString("id")) && contents.contains(QString("state")))
            {
                int id = contents[QString("id")].toInt();
                if(!d->connection_state.contains(id))
                    emit updatePlayer(d->connection_state[id].first, contents[QString("state")]);
            }
        }
    }
}

}
