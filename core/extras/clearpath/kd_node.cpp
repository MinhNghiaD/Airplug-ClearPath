#include "kd_node.h"

namespace ClearPath
{

class Q_DECL_HIDDEN KDNode::Private {
public:
    Private(std::vector<double> nodePos, int splitAxis, int dimension) : position(nodePos), maxRange(nodePos), minRange(nodePos), nbDimension(dimension){
        Q_ASSERT(splitAxis < dimension);
        Q_ASSERT(std::size(nodePos) == dimension);

        this->splitAxis = splitAxis;

        Left = Right = Parent = NULL;
    }

    ~Private(){}

public:
    // TODO: How to return KDNode ?
    KDNode::Private findParent(std::vector<double> nodepos) {
        KDNode::Private parent = NULL;
        KDNode::Private currentNode = this;
        while (currentNode != NULL) {
            currentNode.updateRange(nodepos);

            int split = currentNode.splitAxis;
            parent = currentNode;

            if (nodepos[split] >= currentNode.position[split]) {
                currentNode = currentNode.Right;
            }
            else {
                currentNode = currentNode.Left;
            }
        }

        return parent;
    }

    void updateRange(std::vector<double> pos) {
        if (std::size(pos) != nbDimension) return;
        for (int i = 0; i < nbDimension; i++) {
            maxRange[i] = std::max(maxRange[i], pos[i]);
            minRange[i] = std::min(minRange[i], pos[i]);
        }
    }

public:
    int splitAxis;
    int nbDimension;
    std::vector<double> position;
    std::vector<double> maxRange;
    std::vector<double> minRange;
    KDNode::Private Parent;
    KDNode::Private Left;
    KDNode::Private Right;
};


/* === KDNode Main === */

KDNode::KDNode(std::vector<double> nodePos, int splitAxis, int dimension) :
    d(new Private(nodePos, splitAxis, dimension)) {}

// TODO: KDNode or KDNode::Private ?
KDNode KDNode::insert(std::vector<double> nodePos) {
    if (std::size(nodePos) != d->nbDimension) {
        return;
    }

    KDNode parent = d->findParent(nodePos);

    if (nodePos == parent.getPosition()) {
        return;
    }

    KDNode newNode = new KDNode(nodePos, ((parent.d->splitAxis + 1) % d->nbDimension), d->nbDimension);
    newNode.d->Parent = parent;

    if (nodePos[parent.d->splitAxis] >= parent.d->position[parent.d->splitAxis]) {
        parent.d->Right = newNode;
    }
    else
    {
        parent.d->Left = newNode;
    }

    return newNode;
}

std::vector<double> KDNode::getPosition() {
    return d->position;
}

double getClosestNeighbors(QHash<double, QVector<KDNode>>, std::vector<double>, double, int) {

}

}
