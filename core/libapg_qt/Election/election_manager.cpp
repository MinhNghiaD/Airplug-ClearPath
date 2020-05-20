#include "election_manager.h"
namespace AirPlug
{
class Q_DECL_HIDDEN ElectionManager::Private
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

ElectionManager::ElectionManager()
    : QObject(nullptr),
      d(new Private())
{
    setObjectName(QLatin1String("Election Manager"));
}

ElectionManager::~ElectionManager()
{
    delete d;
}

}
