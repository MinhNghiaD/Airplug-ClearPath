#ifndef COLLISION_AVOIDANCE_MANAGER_H
#define COLLISION_AVOIDANCE_MANAGER_H

//Qt include
#include <QTime>
#include <QMap>
#include <QtMath>

// local include
#include "rvo.h"
#include "kd_tree.h"

namespace ClearPath
{

class CollisionAvoidanceManager
{
public:

    CollisionAvoidanceManager(const std::vector<double>& position,
                              const std::vector<double>& destination,
                              const std::vector<double>& velocity,
                              double timeHorizon,
                              double timeStep,
                              double maxSpeed,
                              double neighborDistance,
                              int    maxNeighbors,
                              KDTree* tree)
    {
    }

    std::vector<double> getPosition()
    {
        return position;
    }

    std::vector<double> getVelocity()
    {
        return velocity;
    }

    void addNeighborOrcaLine(QMap<double, QVector<KDNode>> neighbors)
    {

    }


    void computeNewVelocity()
    {
        orcaLines.clear();

        addNeighborOrcaLine(getClosestNeighbors());

        int lineFail = RVO::checkCollision(orcaLines, maxSpeed, preferenceVelocity, false, newVelocity);

        if (lineFail < orcaLines.size())
        {
            // start optimizing from the first collision
            newVelocity = RVO::collisionFreeVelocity(orcaLines, lineFail, maxSpeed, 0, newVelocity);
        }
    }

    QMap<double, QVector<KDNode>> getClosestNeighbors()
    {
        double searchRange = pow(neighborDistance, 2);

        return obstaclesTree->getClosestNeighbors(position, searchRange, maxNeighbors);
    }

    void update()
    {
        setPreferenceVelocity();
        computeNewVelocity();

        for (int i = 0; i < 2; ++i)
        {
            velocity[i]  = newVelocity[i];
            position[i] += velocity[i] * timeStep;
        }
    }

    void setPreferenceVelocity()
    {
        preferenceVelocity = RVO::vectorSubstract(destination, position);

        double absSqrGoalVector = RVO::vectorProduct(preferenceVelocity, preferenceVelocity);

        if (absSqrGoalVector > 1)
        {
            preferenceVelocity = RVO::scalarProduct(preferenceVelocity, 1/sqrt(absSqrGoalVector));
        }

        /*
         * pivot a little to avoid deadlocks due to perfect symmetry.
         */
        qsrand(QTime::currentTime().msec());

        const double PI       = 3.14159265358979323846f;
        const double angle    = qrand() / (RAND_MAX + 1.0) * 2 * PI;
        const double distance = qrand() / (RAND_MAX + 1.0) * 1;

        preferenceVelocity[0] += distance * cos(angle);
        preferenceVelocity[1] += distance * sin(angle);
    }

    void setDestination(std::vector<double> goal)
    {
        destination = goal;
    }

    bool reachedGoal()
    {
        return true;
    }

private:

    double     timeHorizon;
    double     maxSpeed;
    double     neighborDistance;
    double     timeStep;
    int        maxNeighbors;

    std::vector<double>   position;
    std::vector<double>   destination;
    std::vector<double>   velocity;
    std::vector<double>   newVelocity;
    std::vector<double>   preferenceVelocity;

    // constrain lines
    QVector<Line> orcaLines;
    KDTree*     obstaclesTree;
};

}

#endif // COLLISION_AVOIDANCE_MANAGER_H
