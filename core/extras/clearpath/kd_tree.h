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

/* === FUNCTIONS === */
public:
    bool add(std::vector<double>);
    QHash<double, QVector<KDNode*>>& getClosestNeighbors(std::vector<double>, double, int);

private:
    // If needed
    // class Private;
    // Private* d;

    int nbDimension;
    KDNode* Root;
    QVector<KDNode*> nodeVector;
};

}

#endif // KD_TREE_H
