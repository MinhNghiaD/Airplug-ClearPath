#include "raymond_lock.h"

// Qt include
#include <QVector>

namespace AirPlug
{

class Q_DECL_HIDDEN RaymondLock::Private
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

RaymondLock::RaymondLock()
    : d(new Private())
{
}

RaymondLock::~RaymondLock()
{
    delete d;
}

}
