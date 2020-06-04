#include "kd_node.h"

namespace ClearPath
{

    // TODO: Default construtor
    // KDNode::KDNode() : position(0), maxRange(0), minRange(0), splitAxis(0), nbDimension(1) {}

    KDNode::KDNode(std::vector<double> nodePos, int splitAxis, int dimension) :
            position(nodePos), maxRange(nodePos), minRange(nodePos), splitAxis(splitAxis), nbDimension(dimension) {

        Q_ASSERT(splitAxis < dimension);
        Q_ASSERT(nodePos.size() == dimension);

        this->splitAxis = splitAxis;
    }

    KDNode::~KDNode() {
        delete Parent;
        delete Left;
        delete Right;
    }

    // TODO: Return KDNode& or KDNode* ?
    KDNode KDNode::insert(std::vector<double> nodePos) {
        if (nodePos.size() != this->nbDimension) {
            return *(new KDNode());
        }

        KDNode& parent = *this->findParent(nodePos);

        if (nodePos == parent.getPosition()) {
            return *(new KDNode());
        }

        KDNode& newNode = *(new KDNode(nodePos, ((parent.splitAxis + 1) % this->nbDimension), this->nbDimension));
        newNode.Parent = &parent;

        if (nodePos[parent.splitAxis] >= parent.position[parent.splitAxis]) {
            parent.Right = &newNode;
        }
        else
        {
            parent.Left = &newNode;
        }

        return newNode;
    }

    std::vector<double> KDNode::getPosition() const {
        return this->position;
    }

    // TODO: Convert java to C++
    double KDNode::getClosestNeighbors(const QHash<double, QVector<KDNode>>& nodes, std::vector<double> position, double sqRange, int nbMaxNeighbor) {
        // add current node to the list
        double distanceToCurrentNode = Math.sqrt(sqrDistance(position0, this.position));

        // Don't add self position to list of neighbors
        if (distanceToCurrentNode > 0)
        {
            if (!neighborList.containsKey(distanceToCurrentNode))
            {
                Vector<KDNode> nodes = new Vector<KDNode>();
                nodes.add(this);

                neighborList.put(distanceToCurrentNode, nodes);
            }
            else
            {
                neighborList.get(distanceToCurrentNode).add(this);
            }

            /*
            System.out.println("insert " + Arrays.toString(this.position) +
                       " to the closest neighbors of " + Arrays.toString(position0) +
                       " with distance " + distanceToCurrentNode);
        */
            // limit the size of the Map to maxNbNeighbors
            int size = 0;

            for (Double key : neighborList.keySet())
            {
                size += neighborList.get(key).size();
            }


            if (size > maxNbNeighbors)
            {
                double tailKey = neighborList.lastKey();

                Vector<KDNode> farthesNodes = neighborList.get(tailKey);

                if (farthesNodes.size() == 1)
                {
                    neighborList.remove(tailKey);
                }
                else
                {
                    farthesNodes.remove(farthesNodes.size() - 1);
                }

                // update the searching range
                sqRange = Math.pow(neighborList.lastKey(), 2);
            }
        }

        // sub-trees Traversal
        double sqrDistanceLeftTree  = 0;


        if (Left == null)
        {
            sqrDistanceLeftTree = Double.MAX_VALUE;
        }
        else
        {
            for (int i = 0; i < nbDimension; i++)
            {
                sqrDistanceLeftTree += (Math.pow(Math.max(0, (Left.minRange[i] - position0[i])), 2) +
                                        Math.pow(Math.max(0, (position0[i] - Left.maxRange[i])), 2));
            }
        }


        double sqrDistanceRightTree = 0;

        if (Right == null)
        {
            sqrDistanceRightTree = Double.MAX_VALUE;
        }
        else
        {
            for (int i = 0; i < nbDimension; i++)
            {
                sqrDistanceRightTree += (Math.pow(Math.max(0, (Right.minRange[i] - position0[i])), 2) +
                                         Math.pow(Math.max(0, (position0[i] - Right.maxRange[i])), 2));
            }
        }

        // traverse the closest area
        if (sqrDistanceLeftTree < sqrDistanceRightTree)
        {
            if (sqrDistanceLeftTree < sqRange)
            {
                /*
            System.out.println("left area square distance: " + sqrDistanceLeftTree +
                       " <  square range " + sqRange);
            */
                // traverse Left Tree
                sqRange = Left.getClosestNeighbors(neighborList, position0, sqRange, maxNbNeighbors);

                if (sqrDistanceRightTree < sqRange)
                {
                    /*
                System.out.println("right area square distance: " + sqrDistanceRightTree +
                           " <  square range " + sqRange);
            */
                    // traverse Right Tree
                    sqRange = Right.getClosestNeighbors(neighborList, position0, sqRange, maxNbNeighbors);
                }
            }
        }
        else
        {
            if (sqrDistanceRightTree < sqRange)
            {
                /*
            System.out.println("right area square distance: " + sqrDistanceRightTree +
                       " <  square range " + sqRange);
            */
                // traverse right Tree
                sqRange = Right.getClosestNeighbors(neighborList, position0, sqRange, maxNbNeighbors);

                if (sqrDistanceLeftTree < sqRange)
                {
                    /*
                System.out.println("left area square distance: " + sqrDistanceLeftTree +
                           " <  square range " + sqRange);
            */
                    // traverse Left Tree
                    sqRange = Left.getClosestNeighbors(neighborList, position0, sqRange, maxNbNeighbors);
                }
            }
        }

        return sqRange;
    }

    void KDNode::updateRange(std::vector<double> pos) {
        if (pos.size() != nbDimension) return;
        for (int i = 0; i < nbDimension; i++) {
            maxRange[i] = std::max(maxRange[i], pos[i]);
            minRange[i] = std::min(minRange[i], pos[i]);
        }
    }

    // TODO: Return KDNode& or KDNode* ?
    KDNode KDNode::findParent(std::vector<double> nodepos) {
        KDNode* parent{nullptr};
        KDNode* currentNode = this;
        while (currentNode != nullptr) {
            currentNode->updateRange(nodepos);

            int split = currentNode->splitAxis;
            parent = currentNode;

            if (nodepos[split] >= currentNode->position[split]) {
                currentNode = currentNode->Right;
            }
            else {
                currentNode = currentNode->Left;
            }
        }

        return parent;
    }

}
