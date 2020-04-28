#include "router.h"

namespace AirPlug
{

class Q_DECL_HIDDEN Router::Private
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


Router::Router()
    : d(new Private())
{
}

Router::~Router()
{
    delete d;
}

}
