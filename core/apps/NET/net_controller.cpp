#include "net_controller.h"

//Qt includes
#include <QTimer>
#include <QDebug>

// Local include
#include "laiyang_snapshot.h"
#include "router.h"

using namespace AirPlug;

namespace NetApplication
{

class Q_DECL_HIDDEN NetController::Private
{
public:

    Private()
        : router(nullptr),
          snapshot(nullptr)
    {
    }

    ~Private()
    {
        delete router;
    }

public:
    LaiYangSnapshot* snapshot;
    Router*          router;
};



NetController::NetController(QObject* parent)
    : ApplicationController(QLatin1String("NET"), parent),
      d(new Private())
{
}

NetController::~NetController()
{
    delete d;
}

void NetController::init(const QCoreApplication &app)
{
    ApplicationController::init(app);

    disconnect(m_communication, SIGNAL(signalMessageReceived(Header, Message)),
               this,            SLOT(slotReceiveMessage(Header, Message)));

    d->router   = new Router(m_communication, m_clock->getSiteID());
    d->snapshot = new LaiYangSnapshot();

    d->router->addSnapshot(d->snapshot);
}

void NetController::takeSnapshot() const
{
    d->snapshot->init();
}

void NetController::slotReceiveMessage(Header, Message)
{
}


}
