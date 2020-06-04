#include "kd_tree.h"

namespace ClearPath
{

    /* === Main KDTree === */
    KDTree::KDTree(int dim) : Root(nullptr){ }

    KDTree::~KDTree() {
        delete d;
    }

    bool KDTree::add(std::vector<double> position) {
        if (d->Root == NULL) {
            d->Root = KDNode(position, 0, d->nbDimension);
            d->nodeList.append(d->Root);
        }
        else
        {
            KDNode pNode;
            if ((pNode = d->Root.insert(position)) != NULL) {
                d->nodeList.append(pNode);
            }
        }
        return true;
    }

    QHash<double, QVector<KDNode>> KDTree::getClosestNeighbors(std::vector<double> position, double sqRange, int maxNbNeighbors) {
        QHash<double, QVector<KDNode>> closestNeighbors = new QHash<double, QVector<KDNode>>();
        sqRange = d->Root.getClosestNeighbors(closestNeighbors, position, sqRange, maxNbNeighbors);
        return closestNeighbors;
    }
}
