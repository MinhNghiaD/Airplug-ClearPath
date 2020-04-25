#include "net_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>

// Local include
#include "laiyang_snapshot.h"

using namespace AirPlug;

namespace NetApplication
{

class Q_DECL_HIDDEN NetController::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
    }

public:
    LaiYangSnapshot snapshotManager;
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
        //message = d->snapshotManager.preprocessMessage(message);

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

        // color message before sending to other NET
        //d->snapshotManager.colorMessage(message);
    }

    sendMessage(message, QLatin1String("NET"), app, header.where());
}

}
