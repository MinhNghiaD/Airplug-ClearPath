#include "kd_tree.h"

namespace ClearPath
{

    /* === Main KDTree === */
    KDTree::KDTree(int dim) : Root(nullptr) {}

    KDTree::~KDTree() {}

    bool KDTree::add(std::vector<double> position) {
        if (this->Root == nullptr) {
            this->Root = new KDNode(position, 0, this->nbDimension);
            this->nodeVector.append(this->Root);
        }
        else
        {
            KDNode* pNode;
            if ((pNode = &this->Root->insert(position)) != nullptr) {
                this->nodeVector.append(pNode);
            }
        }
        return true;
    }

    QHash<double, QVector<KDNode*>>& KDTree::getClosestNeighbors(std::vector<double> position, double sqRange, int maxNbNeighbors) {
        QHash<double, QVector<KDNode*>>* closestNeighbors = new QHash<double, QVector<KDNode*>>();
        sqRange = this->Root->getClosestNeighbors(*closestNeighbors, position, sqRange, maxNbNeighbors);
        return closestNeighbors;
    }
}
