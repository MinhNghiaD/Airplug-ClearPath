#include "environment_manager.h"

namespace ClearPath
{

EnvironmentManager* EnvironmentManager::instance = nullptr;


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
                                                                      &obstaclesTree);

    if(obstaclesTree.add(id, agent))
    {
        return agent;
    }

    return nullptr;
}

QMap<QString, CollisionAvoidanceManager *> EnvironmentManager::getAgents() const
{
    return obstaclesTree.getAgents();
}


EnvironmentManager *EnvironmentManager::init()
{
    if (instance)
        delete instance;

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
        delete instance;

    instance = new EnvironmentManager(timeStep,
                                      neighborDistance,
                                      maxNeighbors,
                                      timeHorizon,
                                      radius,
                                      maxSpeed,
                                      velocity);

    return instance;
}

bool EnvironmentManager::setInfo(const QString &name, const QJsonObject &info) const
{
    if (!getAgents().contains(name))
        return false;

    getAgents()[name]->setInfo(info);

    return true;
}


EnvironmentManager *EnvironmentManager::getInstance()
{
    // NOTE instance needs to be init properly
    Q_ASSERT(instance != nullptr);

    return instance;
}

void EnvironmentManager::setTimeStep(double period)
{
    timeStep = period;
}

EnvironmentManager::EnvironmentManager()
    : globalTime(0),
      timeStep(0),
      obstaclesTree(2)
{
}


EnvironmentManager::EnvironmentManager(double timeStep,
                                       double neighborDistance,
                                       int maxNeighbors,
                                       double timeHorizon,
                                       double radius,
                                       double maxSpeed,
                                       std::vector<double> velocity)
    : timeStep(timeStep),
      globalTime(0),
      neighborDistance(neighborDistance),
      maxNeighbors(maxNeighbors),
      timeHorizon(timeHorizon),
      radius(radius),
      maxSpeed(maxSpeed),
      velocity(velocity),
      obstaclesTree(2)
{
}


void EnvironmentManager::doStep()
{
    obstaclesTree.update();
    globalTime += timeStep;
}

double EnvironmentManager::getAgentRadius() const
{
    return radius;
}

}
