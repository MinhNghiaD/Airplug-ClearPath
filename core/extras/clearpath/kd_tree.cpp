#include "kd_tree.h"

namespace ClearPath
{

    class Q_DECL_HIDDEN KDTree::Private {
    public:
        Private(int dim): nbDimension(dim), Root(), nodeList(new QVector<KDNode>){}
        ~Private(){}

    public:
       int nbDimension;
       KDNode Root;
       QVector<KDNode> nodeList;

    };

/* === Main KDTree === */
KDTree::KDTree(int dim) : d(new Private(dim)){ }

bool KDTree::add(double * position) {
    if (d->Root == null) {
        d->Root = new KDNode(position, 0, d->nbDimension);
        d->nodeList.add(d->Root);
    }
    else
    {
        KDNode pNode;
        if ((pNode = d->Root.insert(position)) != nullptr) {
            d->nodeList.add(pNode);
        }
    }
    return true;
}

QHash<double, QVector<KDNode>> KDTree::getClosestNeighbors(double * position, double sqRange, int maxNbNeighbors) {
    QHash<double, QVector<KDNode>> closestNeighbors = new QHash<double, QVector<KDNode>>();
    sqRange = d->Root.getClosestNeighbors(closestNeighbors, position, sqRange, maxNbNeighbors);
    return closestNeighbors;
}
}
