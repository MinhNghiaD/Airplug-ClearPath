#include "laiyang_snapshot.h"

namespace AirPlug
{

class LaiYangSnapshot::Private
{
public:

    Private()
        : recorded(false),
          initiator(false),
          msgCounter(0)
    {
    }

    ~Private()
    {
    }

public:

    bool recorded;
    bool initiator;

    // TODO collect state
    int msgCounter;
};

LaiYangSnapshot::LaiYangSnapshot()
    : d(new Private())
{
}

LaiYangSnapshot::~LaiYangSnapshot()
{
    delete d;
}

}
