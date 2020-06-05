/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-25
 * Description : Node of KD-Tree
 *
 * 2020 by Gaétan Carabetta <carabetta.gaetan at gmail dot com>
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef KD_NODE_H
#define KD_NODE_H

// Std include
#include <vector>

// Qt include
#include <QVector>
#include <QMap>

namespace ClearPath
{

class CollisionAvoidanceManager;

class KDNode
{
public:

    explicit KDNode(CollisionAvoidanceManager* agent, int splitAxis, int dimension);
    ~KDNode();

public:

    KDNode* insert(CollisionAvoidanceManager* agent);

    std::vector<double> getPosition() const;

    CollisionAvoidanceManager* getAgent() const;

    double getClosestNeighbors(QMap<double, QVector<KDNode*> >& neighborList,
                               std::vector<double>              position,
                               double                           sqRange,
                               int                              maxNbNeighbors);

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
