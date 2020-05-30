#include "synchronizer_base.h"
namespace AirPlug
{
class SynchronizerBase::Private
{
public:

    explicit Private(const QString& siteID)
        : siteID(siteID)
    {
    }

    ~Private()
    {
    }

public:

    QString siteID;
};

SynchronizerBase::SynchronizerBase(const QString& siteID)
    : QObject(),
      d(new Private(siteID))
{
}

SynchronizerBase::~SynchronizerBase()
{
    delete d;
}


}
