#ifndef KD_NODE_H
#define KD_NODE_H

#include <QMetaType>
#include <QtGlobal>

#include <vector>
#include <math.h>

namespace ClearPath
{

class KDNode
{
public:
    explicit KDNode(std::vector<double>, int, int);
    ~KDNode();

public:
    KDNode insert(std::vector<double>);
    std::vector<double> getPosition();
    double getClosestNeighbors(QHash<double, QVector<KDNode>>, std::vector<double>, double, int);

    static bool equal(std::vector<double> pos1, std::vector<double> pos2) {
        if (std::size(pos1) != std::size(pos2)) return false;

        for (int k = 0; k < std::size(pos1); k++) {
            if (pos1[k] != pos2[k]) return false;
        }

        return true;
    }

    static double sqrDistance(std::vector<double>, std::vector<double>) {
        if (std::size(pos1) != std::size(pos2)) return -1.;

        double sum = 0.0;

        for (int k = 0; k < std::size(pos1); k++) {
            sum += pow(pos1[k] - pos2[k], 2);
        }

        return sum;
    }

private:
    class Private;
    Private* d;
};

}

#endif // KD_NODE_H
