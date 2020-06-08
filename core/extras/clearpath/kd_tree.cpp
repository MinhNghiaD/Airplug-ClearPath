#include "kd_tree.h"

// Local include
#include "collision_avoidance_manager.h"

namespace ClearPath
{
class KDTree::Private
{
public:
    Private(int dim)
        : nbDimension(dim),
          Root(nullptr)
    {
    }

    ~Private()
    {
        delete Root;

        QMap<QString, CollisionAvoidanceManager*>::iterator agent = agents.begin();

        while (agent != agents.end())
        {
            delete agent.value();
            agents.erase(agent);
        }
    }

public:

    int nbDimension;
    KDNode* Root;
    QMap<QString, CollisionAvoidanceManager*> agents;
};

KDTree::KDTree(int dim)
    : d(new Private(dim))
{
}

KDTree::~KDTree()
{
    delete d;
}

bool KDTree::add(const QString& name, CollisionAvoidanceManager* agent)
{
    if (d->Root == nullptr)
    {
        d->Root = new KDNode(agent, 0, d->nbDimension);

        d->agents[name] = agent;
    }
    else
    {
        if (d->Root->insert(agent) != nullptr)
        {
            d->agents[name] = agent;
        }
    }

    return true;
}

QMap<QString, CollisionAvoidanceManager*> KDTree::getAgents() const
{
    return d->agents;
}

QMap<double, QVector<KDNode*> > KDTree::getClosestNeighbors(const std::vector<double>& position, double sqRange, int maxNbNeighbors) const
{
    QMap<double, QVector<KDNode*> > closestNeighbors;

    sqRange = d->Root->getClosestNeighbors(closestNeighbors, position, sqRange, maxNbNeighbors);

    return closestNeighbors;
}

void KDTree::update()
{
    // clean old tree and construct new one
    delete d->Root;
    d->Root = nullptr;

    for (QMap<QString, CollisionAvoidanceManager*>::iterator agent  = d->agents.begin();
                                                             agent != d->agents.end();
                                                           ++agent)
    {
        add(agent.key(), agent.value());
    }
}

}
