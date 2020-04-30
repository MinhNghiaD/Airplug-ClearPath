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
          snapshot(nullptr),
          pingTimer(nullptr)
    {
    }

    ~Private()
    {
        delete router;
    }

public:
    LaiYangSnapshot* snapshot;
    Router*          router;
    QTimer*          pingTimer;
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

    d->pingTimer = new QTimer(this);

    connect(d->pingTimer, &QTimer::timeout,
            d->router,    &Router::slotHeathCheck, Qt::DirectConnection);

    d->pingTimer->start(20000);
}

void NetController::takeSnapshot() const
{
    d->snapshot->init();
}

void NetController::slotReceiveMessage(Header, Message)
{
}


}
