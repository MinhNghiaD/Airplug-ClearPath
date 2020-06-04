#include "kd_tree.h"

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
    QVector<KDNode*> agents;
};

KDTree::KDTree(int dim)
    : d(new Private(dim))
{
}

KDTree::~KDTree()
{
    delete d;
}

bool KDTree::add(std::vector<double> position)
{
    if (d->Root == nullptr)
    {
        d->Root = new KDNode(position, 0, d->nbDimension);
        d->agents.append(d->Root);
    }
    else
    {
        KDNode* pNode = nullptr;
        if ((pNode = d->Root->insert(position)) != nullptr)
        {
            d->agents.append(pNode);
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
