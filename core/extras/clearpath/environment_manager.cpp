#include "environment_manager.h"

namespace ClearPath
{

class Q_DECL_HIDDEN EnvironmentManager::Private
{
public:

    Private()
        : obstaclesTree(KDTree(2))
    {
    }

    ~Private()
    {
    }

public:

    double   timeStep;
    double   globalTime;
    double   neighborDistance;
    int      maxNeighbors;
    double   timeHorizon;
    double   radius;
    double   maxSpeed;
    std::vector<double> velocity;
    KDTree obstaclesTree;
};

EnvironmentManager* EnvironmentManager::instance = nullptr;

EnvironmentManager *EnvironmentManager::init()
{
    if (instance)
    {
        delete instance;
    }

    instance = new EnvironmentManager();

    return instance;
}

EnvironmentManager* EnvironmentManager::init(double timeStep,
                                             double neighborDistance,
                                             int maxNeighbors,
                                             double timeHorizon,
                                             double radius,
                                             double maxSpeed,
                                             std::vector<double> velocity)
{
    if (instance)
    {
        delete instance;
    }

    instance = new EnvironmentManager(timeStep,
                                      neighborDistance,
                                      maxNeighbors,
                                      timeHorizon,
                                      radius,
                                      maxSpeed,
                                      velocity);

    return instance;
}

void EnvironmentManager::clean()
{
    delete instance;
    instance = nullptr;
}

EnvironmentManager *EnvironmentManager::getInstance()
{
    // NOTE instance needs to be init properly
    Q_ASSERT(instance != nullptr);

    return instance;
}

EnvironmentManager::EnvironmentManager()
    : d(new Private())
{
    d->globalTime = 0;
    d->timeStep   = 0;
}

EnvironmentManager::EnvironmentManager(double timeStep,
                                       double neighborDistance,
                                       int maxNeighbors,
                                       double timeHorizon,
                                       double radius,
                                       double maxSpeed,
                                       std::vector<double> velocity)
    : d(new Private())
{
    // set default parameters
    d->timeStep         = timeStep;
    d->globalTime       = 0;
    d->neighborDistance = neighborDistance;
    d->maxNeighbors     = maxNeighbors;
    d->timeHorizon      = timeHorizon;
    d->radius           = radius;
    d->maxSpeed         = maxSpeed;
    d->velocity         = velocity;
}

EnvironmentManager::~EnvironmentManager()
{
    delete d;
}

CollisionAvoidanceManager * EnvironmentManager::addAgent(const QString& id,
                                                         std::vector<double> position,
                                                         std::vector<double> destination,
                                                         std::vector<double> velocity,
                                                         double timeHorizon,
                                                         double timeStep,
                                                         double maxSpeed,
                                                         double neighborDistance,
                                                         int maxNeighbors)
{
    CollisionAvoidanceManager *agent = new CollisionAvoidanceManager( position,
                                                                      destination,
                                                                      velocity,
                                                                      timeHorizon,
                                                                      timeStep,
                                                                      maxSpeed,
                                                                      neighborDistance,
                                                                      maxNeighbors,
                                                                      &(d->obstaclesTree));

    if(d->obstaclesTree.add(id, agent))
    {
        return agent;
    }

    return nullptr;
}

CollisionAvoidanceManager* EnvironmentManager::addAgent(const QString& id,
                                                        std::vector<double> position,
                                                        std::vector<double> destination)
{
    return addAgent(id, position, destination, d->velocity, d->timeHorizon, d->timeStep, d->maxSpeed, d->neighborDistance, d->maxNeighbors);
}

void EnvironmentManager::update()
{
    d->obstaclesTree.update();
    d->globalTime += d->timeStep;
}

QMap<QString, CollisionAvoidanceManager *> EnvironmentManager::getAgents() const
{
    return d->obstaclesTree.getAgents();
}

void EnvironmentManager::setInfo(const QString &name, const QJsonObject &info) const
{
    if (!getAgents().contains(name))
    {
        CollisionAvoidanceManager *agent = new CollisionAvoidanceManager( {0, 0},
                                                                          {0, 0},
                                                                          {0, 0},
                                                                          d->timeHorizon,
                                                                          d->timeStep,
                                                                          d->maxSpeed,
                                                                          d->neighborDistance,
                                                                          d->maxNeighbors,
                                                                          &(d->obstaclesTree));

        d->obstaclesTree.add(name, agent);
    }

    getAgents()[name]->setInfo(info);
}

void EnvironmentManager::setTimeStep(double period)
{
    d->timeStep = period;
}

double EnvironmentManager::getAgentRadius() const
{
    return d->radius;
}

}
