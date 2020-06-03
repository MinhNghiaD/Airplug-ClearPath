#ifndef KD_TREE_H
#define KD_TREE_H

#include "kd_node.h"

namespace ClearPath
{

class KDTree
{
public:
    explicit KDTree(int);
    ~KDTree();

public:
    bool add(double*);
    QHash<double, QVector<KDNode>> getClosestNeighbors(double*, double, int);

private:
    class Private;
    Private* d;
};

}

#endif // KD_TREE_H
