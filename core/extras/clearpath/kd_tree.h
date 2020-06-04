#ifndef KD_TREE_H
#define KD_TREE_H

#include <QVector>
#include <QMap>

#include "kd_node.h"

namespace ClearPath
{

class KDTree
{
public:

    KDTree(int dim)
    {
    }
/*
    bool add(CollisionAvoidanceManager client)
    {
        return true;
    }
*/
    /**
     *
     * @param position
     * @param sqRange
     * @param maxNbNeighbors
     * @return Map of N-nearest neighbors, sorted by distance
     */
    QMap<double, QVector<KDNode>> getClosestNeighbors(const std::vector<double>& position, double sqRange, int maxNbNeighbors)
    {
        // Map of distance and nodes
        QMap<double, QVector<KDNode>> closestNeighbors;

        return closestNeighbors;
    }

    void update()
    {
    }
/*
    QVector<CollisionAvoidanceManager> getAgents()
    {
        return agents;
    }
*/
    KDNode*                            root;
    int                                nbDimension;
    //QVector<CollisionAvoidanceManager> agents;
};

}

#endif // KD_TREE_H
