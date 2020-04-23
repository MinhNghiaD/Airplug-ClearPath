#include "vector_clock.h"
#include <QtGlobal>

namespace AirPlug
{

class Q_DECL_HIDDEN VectorClock::Private
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

VectorClock::VectorClock()
    : d(new Private())
{

}

VectorClock::~VectorClock()
{
    delete d;
}

}
