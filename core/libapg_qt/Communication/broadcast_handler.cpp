#include "broadcast_handler.h"

namespace AirPlug
{

class Q_DECL_HIDDEN BroadcastHandler::Private
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


BroadcastHandler::BroadcastHandler()
    : d(new Private())
{
}

BroadcastHandler::~BroadcastHandler()
{
    delete d;
}

}
