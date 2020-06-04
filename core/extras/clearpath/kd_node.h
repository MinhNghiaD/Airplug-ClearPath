#ifndef KD_NODE_H
#define KD_NODE_H

#include <QMetaType>
#include <QtGlobal>
#include <QVector>
#include <QHash>

#include <vector>
#include <math.h>

namespace ClearPath
{

class KDNode
{
public:
    KDNode() = default;
    KDNode(std::vector<double>, int, int);
    ~KDNode();

/* === FUNCTIONS === */
public:
    // TODO: Return KDNode& or KDNode* ?
    KDNode insert(std::vector<double>);

    std::vector<double> getPosition() const;
    double getClosestNeighbors(const QHash<double, QVector<KDNode>>&, std::vector<double>, double, int);

    static bool equal(std::vector<double> pos1, std::vector<double> pos2) {
        /*
        if (pos1.size() != pos2.size()) return false;

        for (int k = 0; k < pos1.size(); k++) {
            if (pos1[k] != pos2[k]) return false;
        }

        return true;
        */
        return pos1 == pos2;
    }

    static double sqrDistance(std::vector<double> pos1, std::vector<double> pos2) {
        if (pos1.size() != pos2.size()) return -1.;

        double sum = 0.0;

        for (auto x{pos1.cbegin()}, y{pos2.cbegin()}; x != pos1.end() && y != pos2.end(); x++, y++) {
            sum += pow(x - y, 2);
        }

        return sum;
    }

private:
    void updateRange(std::vector<double>);

    // TODO: return KDNode& or KDNode* ?
    KDNode findParent(std::vector<double>);

private:
    // If needed
    // class Private;
    // Private* d;

    int splitAxis;
    int nbDimension;
    std::vector<double> position;
    std::vector<double> maxRange;
    std::vector<double> minRange;
    KDNode* Parent;
    KDNode* Left;
    KDNode* Right;
};

}

#endif // KD_NODE_H
