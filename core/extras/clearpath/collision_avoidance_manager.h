/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-6-4
 * Description : Interface with clearpath algorithm for agents
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef COLLISION_AVOIDANCE_MANAGER_H
#define COLLISION_AVOIDANCE_MANAGER_H

//Qt include
#include <QTime>
#include <QMap>
#include <QtMath>
#include <QJsonArray>
#include <QJsonObject>

// local include
#include "rvo.h"
#include "kd_tree.h"

namespace ClearPath
{

class CollisionAvoidanceManager
{
public:

    explicit CollisionAvoidanceManager(const std::vector<double>& position,
                                       const std::vector<double>& destination,
                                       const std::vector<double>& velocity,
                                       double timeHorizon,
                                       double timeStep,
                                       double maxSpeed,
                                       double neighborDistance,
                                       int    maxNeighbors,
                                       KDTree* tree)
    {
        // Prototype
    }

    std::vector<double> getPosition()
    {
        return position;
    }

    std::vector<double> getVelocity()
    {
        return velocity;
    }

    void addNeighborOrcaLine(QMap<double, QVector<KDNode*>> neighbors)
    {
        // Prototype
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

    QMap<double, QVector<KDNode*>> getClosestNeighbors()
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

        const double angle    = qrand() / (RAND_MAX) * 2 * M_PI;
        const double distance = qrand() / (RAND_MAX) * 1;

        preferenceVelocity[0] += distance * cos(angle);
        preferenceVelocity[1] += distance * sin(angle);
    }

    void setDestination(std::vector<double> goal)
    {
        destination = goal;
    }

    bool reachedGoal()
    {
        // Prototype
        return true;
    }

    /**
     * @brief setInfo : use for updating information of distance agent
     * @param position
     * @param velocity
     * @param maxSpeed
     */
    bool setInfo(QJsonObject info)
    {
        bool success = false;
        success = decodeVector(info[QLatin1String("position")].toArray(), position);
        success = decodeVector(info[QLatin1String("velocity")].toArray(), velocity);

        maxSpeed = info[QLatin1String("maxSpeed")].toDouble();

        return success;
    }

    QJsonObject getInfo()
    {
        QJsonObject info;

        info[QLatin1String("position")] = encodeVector(position);
        info[QLatin1String("velocity")] = encodeVector(velocity);
        info[QLatin1String("maxSpeed")] = maxSpeed;

        return info;
    }

    QJsonObject captureState()
    {
        QJsonObject state;
        /* TODO : capture:
        double     timeHorizon;
        double     maxSpeed;
        double     neighborDistance;
        double     timeStep;
        int        maxNeighbors;

        std::vector<double> position;
        std::vector<double> destination;
        std::vector<double> velocity;
        std::vector<double> newVelocity;
        std::vector<double> preferenceVelocity;
        into QjsonObject in the same way as getInfo() for snapshot
        */

        return state;
    }

private:

    static QJsonArray encodeVector(const std::vector<double>& vector)
    {
        QJsonArray array;

        for (size_t i = 0; i < vector.size(); ++i)
        {
            array << vector[i];
        }

        return array;
    }

    static bool decodeVector(const QJsonArray& json, std::vector<double>& vector)
    {
        if (json.size() != vector.size())
        {
            return false;
        }

        for (int i = 0; i < vector.size(); ++i)
        {
            vector[i] = json[i].toDouble();
        }

        return true;
    }


private:

    double     timeHorizon;
    double     maxSpeed;
    double     neighborDistance;
    double     timeStep;
    int        maxNeighbors;

    std::vector<double> position;
    std::vector<double> destination;
    std::vector<double> velocity;
    std::vector<double> newVelocity;
    std::vector<double> preferenceVelocity;

    // constrain lines
    std::vector<Line>   orcaLines;
    KDTree*             obstaclesTree;
};

}

#endif // COLLISION_AVOIDANCE_MANAGER_H
