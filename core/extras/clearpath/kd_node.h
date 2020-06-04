#ifndef KD_NODE_H
#define KD_NODE_H

//#include <QMetaType>
//#include <QtGlobal>
#include <QVector>
#include <QMap>

#include <vector>


namespace ClearPath
{

class KDNode
{
public:

    explicit KDNode(const std::vector<double>& nodePos, int splitAxis, int dimension);
    ~KDNode();

public:

    KDNode* insert(const std::vector<double>& nodePos);

    std::vector<double> getPosition() const;

    double getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                               std::vector<double> position,
                               double sqRange,
                               int maxNbNeighbors);

    static double sqrDistance(std::vector<double> pos1, std::vector<double> pos2);

private:
    void updateRange(std::vector<double>);

    KDNode* findParent(std::vector<double> nodePos);

private:

    class Private;
    Private* d;
};

}

#endif // KD_NODE_H
