#include "synchronizer.h"
namespace AirPlug
{
class Synchronizer::Private
{
public:

    explicit Private()
    {
    }

    ~Private()
    {
    }

public:

};

Synchronizer::Synchronizer()
    : QObject(),
      d(new Private())
{
}

Synchronizer::~Synchronizer()
{
    delete d;
}

}
