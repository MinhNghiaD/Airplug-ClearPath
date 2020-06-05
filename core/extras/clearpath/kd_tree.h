#ifndef KD_TREE_H
#define KD_TREE_H

#include "kd_node.h"

namespace ClearPath
{

class CollisionAvoidanceManager;

class KDTree
{
public:

    explicit KDTree(int dim);
    ~KDTree();

    /**
     *
     * @param position
     * @param sqRange
     * @param maxNbNeighbors
     * @return Map of N-nearest neighbors, sorted by distance
     */
    QMap<double, QVector<KDNode*> > getClosestNeighbors(const std::vector<double>& position, double sqRange, int maxNbNeighbors) const;

    void update();

    bool add(const QString& name, CollisionAvoidanceManager* agent);

    QMap<QString, CollisionAvoidanceManager*> getAgents() const;

private:

    class Private;
    Private* d;
};

}

#endif // KD_TREE_H
