#include "net_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>


using namespace AirPlug;

namespace NetApplication
{

class NetController::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
    }

public:

};



NetController::NetController(QObject* parent)
    : ApplicationController(QLatin1String("NET"), parent),
      d(new Private)
{
}

NetController::~NetController()
{
    delete d;
}

void NetController::slotReceiveMessage(Header header, Message message)
{
    QString app;

    if (header.what() == QLatin1String("NET"))
    {
        // receive from other NET
        // remove this pair and move to app
        QHash<QString, QString> contents = message.getContents();

        app = contents[QLatin1String("appnet")];

        contents.remove(QLatin1String("appnet"));

        message = Message(contents);
    }
    else
    {
        // receive from other applications
        // TODO adapt with different type of application
        message.addContent(QLatin1String("appnet"), header.what());

        app = QLatin1String("NET");
    }

    sendMessage(message, QLatin1String("NET"), app, header.where());
}

}
