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

        if (d->agents.contains(name))
        {
            d->agents[name] = agent;
        }
    }
    else
    {
        if (d->Root->insert(agent) != nullptr)
        {
            if (d->agents.contains(name))
            {
                d->agents[name] = agent;
            }
        }
    }

    return true;
}

QMap<double, QVector<KDNode*> > KDTree::getClosestNeighbors(const std::vector<double>& position, double sqRange, int maxNbNeighbors)
{
    QMap<double, QVector<KDNode*> > closestNeighbors;

    sqRange = d->Root->getClosestNeighbors(closestNeighbors, position, sqRange, maxNbNeighbors);

    return closestNeighbors;
}
}
