#include "kd_tree.h"

#include <QDebug>

using namespace ClearPath;

int main(int argc, char *argv[])
{
    KDTree tree(2);

    tree.add({ 11, 2 });
    tree.add({ 2, 12 });
    tree.add({ 3, 12 });
    tree.add({ 25, 0 });
    tree.add({ 12, 1 });
    tree.add({ 11, 4 });
    tree.add({ 2, 6 });
    tree.add({ 6, 2 });

    std::vector<double> currentPosition = { 3, 55 };
    QMap<double, QVector<KDNode*> > closestNodes = tree.getClosestNeighbors(currentPosition, 1000, 2);

    qDebug() << "Closest neighbors to " << currentPosition << ":";

    for (QMap<double, QVector<KDNode*> >::const_iterator iter  = closestNodes.cbegin();
                                                         iter != closestNodes.cend();
                                                       ++iter)
    {
        for (QVector<KDNode*>::const_iterator node  = iter.value().cbegin();
                                              node != iter.value().cend();
                                            ++node)
        {
            qDebug() << (*node)->getPosition()  << ", distance = " << iter.key();
        }
    }
}
