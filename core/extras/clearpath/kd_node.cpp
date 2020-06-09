#include "kd_node.h"

// std include
#include <cfloat>

// Qt include
#include <QtMath>

// Local include
#include "collision_avoidance_manager.h"

namespace ClearPath
{

class Q_DECL_HIDDEN KDNode::Private
{
public:

    Private(CollisionAvoidanceManager* agent, int splitAxis, int dimension)
        : agent(agent),
          splitAxis(splitAxis),
          nbDimension(dimension),
          position(agent->getPosition()),
          maxRange(agent->getPosition()),
          minRange(agent->getPosition()),
          Parent(nullptr),
          Left(nullptr),
          Right(nullptr)
    {
    }

    ~Private()
    {
        delete Left;
        delete Right;
    }

public:

    CollisionAvoidanceManager* agent;

    int splitAxis;
    int nbDimension;
    std::vector<double> position;
    std::vector<double> maxRange;
    std::vector<double> minRange;
    KDNode* Parent;
    KDNode* Left;
    KDNode* Right;
};


double KDNode::sqrDistance(std::vector<double> pos1, std::vector<double> pos2)
{
    Q_ASSERT(pos1.size() == pos2.size());

    double sqrDistance = 0;

    for (size_t i = 0; i < pos1.size(); ++i)
    {
        sqrDistance += pow((pos1[i] - pos2[i]), 2);
    }

    return sqrDistance;
}

KDNode::KDNode(CollisionAvoidanceManager* agent, int splitAxis, int dimension)
    : d(new Private(agent, splitAxis, dimension))
{
    Q_ASSERT(splitAxis < dimension);
    Q_ASSERT(int(agent->getPosition().size()) == dimension);
}

KDNode::~KDNode()
{
    delete d;
}

KDNode* KDNode::insert(CollisionAvoidanceManager* agent)
{
    std::vector<double> nodePos = agent->getPosition();

    if (int(nodePos.size()) != d->nbDimension)
    {
        return nullptr;
    }

    KDNode* parent = findParent(nodePos);
/*
    if (nodePos == parent->getPosition())
    {
        return nullptr;
    }
*/
    KDNode* newNode = new KDNode(agent,
                                 ((parent->d->splitAxis + 1) % d->nbDimension),
                                 d->nbDimension);
    newNode->d->Parent = parent;

    if (nodePos[parent->d->splitAxis] >= parent->getPosition()[parent->d->splitAxis])
    {
        parent->d->Right = newNode;
    }
    else
    {
        parent->d->Left = newNode;
    }

    return newNode;
}

std::vector<double> KDNode::getPosition() const
{
    return d->position;
}

CollisionAvoidanceManager* KDNode::getAgent() const
{
    return d->agent;
}

double KDNode::getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                                   std::vector<double>              position,
                                   double                           sqRange,
                                   int                              maxNbNeighbors)
{
    // add current node to the list
    double distanceToCurrentNode = sqrt(sqrDistance(position, d->position));

    // Don't add self position to list of neighbors
    if (distanceToCurrentNode > 0)
    {
        neighborList[distanceToCurrentNode].append(this);

        // limit the size of the Map to maxNbNeighbors
        int size = 0;

        for (QMap<double, QVector<KDNode*> >::const_iterator iter  = neighborList.cbegin();
                                                             iter != neighborList.cend();
                                                           ++iter)
        {
            size += iter.value().size();
        }


        if (size > maxNbNeighbors)
        {
            // Eliminate the farthest neighbor
            QMap<double, QVector<KDNode*> >::iterator farthestNodes = (neighborList.end() - 1);

            if (farthestNodes.value().size() == 1)
            {
                neighborList.erase(farthestNodes);
            }
            else
            {
                farthestNodes.value().pop_back();
            }

            // update the searching range
            sqRange = pow(neighborList.lastKey(), 2);
        }
    }

    // sub-trees Traversal
    double sqrDistanceLeftTree  = 0;


    if (d->Left == nullptr)
    {
        sqrDistanceLeftTree = DBL_MAX;
    }
    else
    {
        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistanceLeftTree += (pow(qMax(0.0, (d->Left->d->minRange[i] - position[i])), 2) +
                                    pow(qMax(0.0, (position[i] - d->Left->d->maxRange[i])), 2));
        }
    }


    double sqrDistanceRightTree = 0;

    if (d->Right == nullptr)
    {
        sqrDistanceRightTree = DBL_MAX;
    }
    else
    {
        for (int i = 0; i < d->nbDimension; ++i)
        {
            sqrDistanceRightTree += (pow(qMax(0.0, (d->Right->d->minRange[i] - position[i])), 2) +
                                     pow(qMax(0.0, (position[i] - d->Right->d->maxRange[i])), 2));
        }
    }

    // traverse the closest area
    if (sqrDistanceLeftTree < sqrDistanceRightTree)
    {
        if (sqrDistanceLeftTree < sqRange)
        {
            // traverse Left Tree
            sqRange = d->Left->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistanceRightTree < sqRange)
            {
                // traverse Right Tree
                sqRange = d->Right->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }
    else
    {
        if (sqrDistanceRightTree < sqRange)
        {
            // traverse right Tree
            sqRange = d->Right->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);

            if (sqrDistanceLeftTree < sqRange)
            {
                // traverse Left Tree
                sqRange = d->Left->getClosestNeighbors(neighborList, position, sqRange, maxNbNeighbors);
            }
        }
    }

    return sqRange;
}

void KDNode::updateRange(std::vector<double> pos)
{
    if (int(pos.size()) != d->nbDimension)
    {
        return;
    }

    for (int i = 0; i < d->nbDimension; i++)
    {
        d->maxRange[i] = std::max(d->maxRange[i], pos[i]);
        d->minRange[i] = std::min(d->minRange[i], pos[i]);
    }
}

KDNode* KDNode::findParent(std::vector<double> nodePos)
{
    KDNode* parent = nullptr;
    KDNode* currentNode = this;

    while (currentNode != nullptr)
    {
        currentNode->updateRange(nodePos);

        int split = currentNode->d->splitAxis;
        parent    = currentNode;

        if (nodePos[split] >= currentNode->d->position[split])
        {
            currentNode = currentNode->d->Right;
        }
        else
        {
            currentNode = currentNode->d->Left;
        }
    }

    return parent;
}

}
