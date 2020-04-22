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
        // TODO: get default message ffrom QSettings
        //messageToSend = QLatin1String("~");
    }

    ~Private()
    {
        //delete timer;
    }

public:

    //QTimer*               timer;

    //QString               messageToSend;
    //int                   nbSequence;
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
    // add key-value appnet
    message.addContent(QLatin1String("appnet"), header.what());

    qDebug() << "NET receives: " << message.getMessage();

    sendMessage(message, header.what(), header.who(), header.where());
}

}
