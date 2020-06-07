#include "collision_avoidance_manager.h"

#include <math.h>

namespace ClearPath
{

    CollisionAvoidanceManager::CollisionAvoidanceManager(const std::vector<double>& position,
                                       const std::vector<double>& destination,
                                       const std::vector<double>& velocity,
                                       double timeHorizon,
                                       double timeStep,
                                       double maxSpeed,
                                       double neighborDistance,
                                       int    maxNeighbors,
                                       KDTree* tree)
        : position(position),
          destination(destination),
          velocity(velocity),
          timeHorizon(timeHorizon),
          timeStep(timeStep),
          maxSpeed(maxSpeed),
          neighborDistance(neighborDistance),
          maxNeighbors(maxNeighbors),
          obstaclesTree(tree),
          orcaLines(std::vector<Line>())
    {
        // Prototype
    }

    std::vector<double> CollisionAvoidanceManager::getPosition()
    {
        return position;
    }

    std::vector<double> CollisionAvoidanceManager::getVelocity()
    {
        return velocity;
    }

    void CollisionAvoidanceManager::addNeighborOrcaLine(QMap<double, QVector<KDNode*>> neighbors)
    {
        const double invTimeHorizon = 1 / timeHorizon;

        for (double key : neighbors.keys())
        {
            for (KDNode* node : neighbors.value(key))
            {

                CollisionAvoidanceManager* neighbor = node->getAgent();

                const std::vector<double> relativePosition = RVO::vectorSubstract(neighbor->getPosition(), position);
                const std::vector<double> relativeVelocity = RVO::vectorSubstract(velocity, neighbor->getVelocity());

                const double distSq = RVO::vectorProduct(relativePosition, relativePosition);
                const double combinedmaxSpeed = maxSpeed + neighbor->maxSpeed;
                const double combinedmaxSpeedSq = pow(combinedmaxSpeed, 2);

                std::vector<double> p1(2);
                std::vector<double> p2(2);
                Line line(p1, p2);
                std::vector<double> u(2);

                if (distSq > combinedmaxSpeedSq)
                {
                    /* No collision. */
                    const std::vector<double> w = RVO::vectorSubstract(relativeVelocity,
                            RVO::scalarProduct(relativePosition, invTimeHorizon));
                    /* Vector from cutoff center to relative velocity. */
                    const double wLengthSq = RVO::vectorProduct(w, w);

                    const double dotProduct1 = RVO::vectorProduct(w, relativePosition);

                    if ((dotProduct1 < 0.0f) && (pow(dotProduct1, 2) > (combinedmaxSpeedSq * wLengthSq)))
                    {
                        /* Project on cut-off circle. */
                        const double wLength = sqrt(wLengthSq);
                        const std::vector<double> unitW = RVO::scalarProduct(w, 1 / wLength);

                        line.direction[0] = unitW[1];
                        line.direction[1] = -unitW[0];

                        u = RVO::scalarProduct(unitW, (combinedmaxSpeed * invTimeHorizon - wLength));
                    }
                    else
                    {
                        /* Project on legs. */
                        const double leg = sqrt(distSq - combinedmaxSpeedSq);

                        if (RVO::det2D(relativePosition, w) > 0.0f)
                        {
                            /* Project on left leg. */
                            line.direction[0] = (relativePosition[0] * leg - relativePosition[1] * combinedmaxSpeed) / distSq;
                            line.direction[1] = (relativePosition[1] * leg + relativePosition[0] * combinedmaxSpeed) / distSq;
                        }
                        else
                        {
                            /* Project on right leg. */
                            line.direction[0] = -(relativePosition[0] * leg + relativePosition[1] * combinedmaxSpeed) / distSq;
                            line.direction[1] = -(relativePosition[1] * leg - relativePosition[0] * combinedmaxSpeed) / distSq;
                        }

                        const double dotProduct2 = RVO::vectorProduct(relativeVelocity, line.direction);

                        u = RVO::vectorSubstract(RVO::scalarProduct(line.direction, dotProduct2), relativeVelocity);
                    }
                }
                else
                {
                    /* Collision. Project on cut-off circle of time timeStep. */
                    const double invTimeStep = 1.0 / timeStep;

                    /* Vector from cutoff center to relative velocity. */
                    const std::vector<double> w = RVO::vectorSubstract(relativeVelocity, RVO::scalarProduct(relativePosition, invTimeStep));

                    const double wLength = sqrt(RVO::vectorProduct(w, w));
                    const std::vector<double> unitW = RVO::scalarProduct(w, 1 / wLength);

                    line.direction[0] =  unitW[1];
                    line.direction[1] = -unitW[0];

                    u = RVO::scalarProduct(unitW, (combinedmaxSpeed * invTimeStep - wLength));
                }

                line.point[0] = velocity[0] + u[0] * 0.5;
                line.point[1] = velocity[1] + u[1] * 0.5;

                orcaLines.push_back(line);
            }
        }
    }

    void CollisionAvoidanceManager::computeNewVelocity()
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

    QMap<double, QVector<KDNode*>> CollisionAvoidanceManager::getClosestNeighbors()
    {
        double searchRange = pow(neighborDistance, 2);

        return obstaclesTree->getClosestNeighbors(position, searchRange, maxNeighbors);
    }

    void CollisionAvoidanceManager::update()
    {
        setPreferenceVelocity();
        computeNewVelocity();

        for (int i = 0; i < 2; ++i)
        {
            velocity[i]  = newVelocity[i];
            position[i] += velocity[i] * timeStep;
        }
    }

    void CollisionAvoidanceManager::setPreferenceVelocity()
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

    void CollisionAvoidanceManager::setDestination(std::vector<double> goal)
    {
        destination = goal;
    }

    bool CollisionAvoidanceManager::reachedGoal()
    {
        std::vector<double> distanceToGoal = RVO::vectorSubstract(position, destination);

        const double sqrDistance = RVO::vectorProduct(distanceToGoal, distanceToGoal);

        if (sqrDistance > 400)
        {
            return false;
        }

        return true;
    }

    bool CollisionAvoidanceManager::setInfo(QJsonObject info)
    {
        bool success = false;
        success = decodeVector(info[QLatin1String("position")].toArray(), position);
        success = decodeVector(info[QLatin1String("velocity")].toArray(), velocity);

        maxSpeed = info[QLatin1String("maxSpeed")].toDouble();

        return success;
    }

    QJsonObject CollisionAvoidanceManager::getInfo()
    {
        QJsonObject info;

        info[QLatin1String("position")] = encodeVector(position);
        info[QLatin1String("velocity")] = encodeVector(velocity);
        info[QLatin1String("maxSpeed")] = maxSpeed;

        return info;
    }

    QJsonObject CollisionAvoidanceManager::captureState()
    {
        QJsonObject state;
        state[QLatin1String("timeHorizon")] = timeHorizon;
        state[QLatin1String("maxSpeed")] = maxSpeed;
        state[QLatin1String("neighborDistance")] = neighborDistance;
        state[QLatin1String("timeStep")] = timeStep;
        state[QLatin1String("maxNeighbors")] = maxNeighbors;
        state[QLatin1String("position")] = encodeVector(position);
        state[QLatin1String("destination")] = encodeVector(destination);
        state[QLatin1String("velocity")] = encodeVector(velocity);
        state[QLatin1String("newVelocity")] = encodeVector(newVelocity);
        state[QLatin1String("preferenceVelocity")] = encodeVector(preferenceVelocity);
        return state;
    }

    QJsonArray CollisionAvoidanceManager::encodeVector(const std::vector<double>& vector)
    {
        QJsonArray array;

        for (size_t i = 0; i < vector.size(); ++i)
        {
            array << vector[i];
        }

        return array;
    }

    bool CollisionAvoidanceManager::decodeVector(const QJsonArray& json, std::vector<double>& vector)
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

}
