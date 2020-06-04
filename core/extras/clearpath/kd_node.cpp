#include "kd_node.h"

namespace ClearPath
{

    KDNode::KDNode() : position(0), maxRange(0), minRange(0), splitAxis(0), nbDimension(1) {}

    KDNode::KDNode(std::vector<double> nodePos, int splitAxis, int dimension) :
            position(nodePos), maxRange(nodePos), minRange(nodePos), splitAxis(splitAxis), nbDimension(dimension) {

        Q_ASSERT(splitAxis < dimension);
        Q_ASSERT(nodePos.size() == dimension);

        this->splitAxis = splitAxis;
    }

    KDNode::~KDNode() {
        delete d;
    }

    KDNode KDNode::insert(std::vector<double> nodePos) {
        if (nodePos.size() != d->nbDimension) {
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

    double KDNode::getClosestNeighbors(QHash<double, QVector<KDNode>>, std::vector<double>, double, int) {

    }

    void KDNode::updateRange(std::vector<double> pos) {
        if (pos.size() != nbDimension) return;
        for (int i = 0; i < nbDimension; i++) {
            maxRange[i] = std::max(maxRange[i], pos[i]);
            minRange[i] = std::min(minRange[i], pos[i]);
        }
    }

    KDNode KDNode::findParent(std::vector<double> nodepos) {
        KDNode parent{};
        KDNode currentNode = this;
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

}
