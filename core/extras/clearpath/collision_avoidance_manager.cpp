#include "collision_avoidance_manager.h"

// std incldue
#include <math.h>
#include <random>

// Qt include
#include <QTime>

// Local include
#include "rvo.h"

namespace ClearPath
{
class Q_DECL_HIDDEN CollisionAvoidanceManager::Private
{
public:
    Private(const std::vector<double>& position,
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
          isFinished(false),
          obstaclesTree(tree)
    {
    }

    ~Private()
    {
    }

public:

    std::vector<double> position;
    std::vector<double> destination;
    std::vector<double> velocity;
    std::vector<double> newVelocity;
    std::vector<double> preferenceVelocity;

    double timeHorizon;
    double maxSpeed;
    double neighborDistance;
    double timeStep;
    int    maxNeighbors;
    bool   isFinished;

    // constrain lines
    std::vector<Line>   orcaLines;
    KDTree*             obstaclesTree;
};


CollisionAvoidanceManager::CollisionAvoidanceManager(const std::vector<double>& position,
                                                     const std::vector<double>& destination,
                                                     const std::vector<double>& velocity,
                                                     double timeHorizon,
                                                     double timeStep,
                                                     double maxSpeed,
                                                     double neighborDistance,
                                                     int    maxNeighbors,
                                                     KDTree* tree)
    : d(new Private(position, destination, velocity,
                    timeHorizon, timeStep, maxSpeed,
                    neighborDistance, maxNeighbors, tree))
{
}

CollisionAvoidanceManager::~CollisionAvoidanceManager()
{
    delete d;
}

void CollisionAvoidanceManager::update()
{
    setPreferenceVelocity();
    computeNewVelocity();

    for (int i = 0; i < 2; ++i)
    {
        d->velocity[i]  = d->newVelocity[i];
        d->position[i] += d->velocity[i] * d->timeStep;
    }
}

std::vector<double> CollisionAvoidanceManager::getPosition()
{
    return d->position;
}

std::vector<double> CollisionAvoidanceManager::getVelocity()
{
    return d->velocity;
}

void CollisionAvoidanceManager::setDestination(std::vector<double> goal)
{
    d->destination = goal;
}

bool CollisionAvoidanceManager::setInfo(QJsonObject info)
{
    bool success = false;
    success = decodeVector(info[QLatin1String("position")].toArray(), d->position);
    success = decodeVector(info[QLatin1String("velocity")].toArray(), d->velocity);

    d->maxSpeed   = info[QLatin1String("maxSpeed")].toDouble();
    d->isFinished = info[QLatin1String("finished")].toBool();

    return success;
}

QJsonObject CollisionAvoidanceManager::getInfo()
{
    QJsonObject info;

    info[QLatin1String("position")] = encodeVector(d->position);
    info[QLatin1String("velocity")] = encodeVector(d->velocity);
    info[QLatin1String("maxSpeed")] = d->maxSpeed;
    info[QLatin1String("finished")] = d->isFinished;

    return info;
}

QJsonObject CollisionAvoidanceManager::captureState()
{
    QJsonObject state;
    state[QLatin1String("timeHorizon")]        = d->timeHorizon;
    state[QLatin1String("maxSpeed")]           = d->maxSpeed;
    state[QLatin1String("neighborDistance")]   = d->neighborDistance;
    state[QLatin1String("timeStep")]           = d->timeStep;
    state[QLatin1String("maxNeighbors")]       = d->maxNeighbors;
    state[QLatin1String("position")]           = encodeVector(d->position);
    state[QLatin1String("destination")]        = encodeVector(d->destination);
    state[QLatin1String("velocity")]           = encodeVector(d->velocity);
    state[QLatin1String("newVelocity")]        = encodeVector(d->newVelocity);
    state[QLatin1String("preferenceVelocity")] = encodeVector(d->preferenceVelocity);

    return state;
}

void CollisionAvoidanceManager::addNeighborOrcaLine(QMap<double, QVector<KDNode*> > neighbors)
{
    const double invTimeHorizon = 1 / d->timeHorizon;

    for (double key : neighbors.keys())
    {
        for (KDNode* node : neighbors.value(key))
        {
            CollisionAvoidanceManager* neighbor = node->getAgent();

            const std::vector<double> relativePosition = RVO::vectorSubstract(neighbor->getPosition(), d->position);
            const std::vector<double> relativeVelocity = RVO::vectorSubstract(d->velocity, neighbor->getVelocity());

            const double distSq             = RVO::vectorProduct(relativePosition, relativePosition);
            const double combinedmaxSpeed   = d->maxSpeed + neighbor->d->maxSpeed;
            const double combinedmaxSpeedSq = pow(combinedmaxSpeed, 2);

            Line line(std::vector<double>(2), std::vector<double>(2));
            std::vector<double> u(2);

            if (distSq > combinedmaxSpeedSq)
            {
                /* No collision. */
                const std::vector<double> w = RVO::vectorSubstract(relativeVelocity,
                                                                   RVO::scalarProduct(relativePosition, invTimeHorizon));
                /* Vector from cutoff center to relative velocity. */
                const double wLengthSq = RVO::vectorProduct(w, w);

                const double dotProduct1 = RVO::vectorProduct(w, relativePosition);

                if ((dotProduct1 < 0.0) && (pow(dotProduct1, 2) > (combinedmaxSpeedSq * wLengthSq)))
                {
                    /* Project on cut-off circle. */
                    const double wLength            = sqrt(wLengthSq);
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
                const double invTimeStep = 1.0 / (d->timeStep);

                /* Vector from cutoff center to relative velocity. */
                const std::vector<double> w = RVO::vectorSubstract(relativeVelocity,
                                                                   RVO::scalarProduct(relativePosition, invTimeStep));

                const double wLength            = sqrt(RVO::vectorProduct(w, w));
                const std::vector<double> unitW = RVO::scalarProduct(w, 1 / wLength);

                line.direction[0] =  unitW[1];
                line.direction[1] = -unitW[0];

                u = RVO::scalarProduct(unitW, (combinedmaxSpeed * invTimeStep - wLength));
            }

            line.point[0] = d->velocity[0] + u[0] * 0.5;
            line.point[1] = d->velocity[1] + u[1] * 0.5;

            d->orcaLines.push_back(line);
        }
    }
}

void CollisionAvoidanceManager::computeNewVelocity()
{
    d->orcaLines.clear();

    addNeighborOrcaLine(getClosestNeighbors());

    int lineFail = RVO::checkCollision(d->orcaLines, d->maxSpeed, d->preferenceVelocity, false, d->newVelocity);

    if (lineFail < d->orcaLines.size())
    {
        // start optimizing from the first collision
        d->newVelocity = RVO::collisionFreeVelocity(d->orcaLines, lineFail, d->maxSpeed, 0, d->newVelocity);
    }
}

QMap<double, QVector<KDNode*> > CollisionAvoidanceManager::getClosestNeighbors()
{
    double searchRange = pow(d->neighborDistance, 2);

    return d->obstaclesTree->getClosestNeighbors(d->position, searchRange, d->maxNeighbors);
}


void CollisionAvoidanceManager::setPreferenceVelocity()
{
    d->preferenceVelocity = RVO::vectorSubstract(d->destination, d->position);

    double absSqrGoalVector = RVO::vectorProduct(d->preferenceVelocity, d->preferenceVelocity);

    if (absSqrGoalVector > 1)
    {
        d->preferenceVelocity = RVO::scalarProduct(d->preferenceVelocity, 1/sqrt(absSqrGoalVector));
    }

    /*
     * pivot a little to avoid deadlocks due to perfect symmetry.
     */

    std::random_device seeder{};
    std::mt19937 gen{seeder()};

    std::normal_distribution<> random(0, RAND_MAX);

    const double angle    = random(gen) / (RAND_MAX) * 2 * M_PI;
    const double distance = random(gen) / (RAND_MAX) * 0.1;

    d->preferenceVelocity[0] += distance * cos(angle);
    d->preferenceVelocity[1] += distance * sin(angle);
}

bool CollisionAvoidanceManager::reachedGoal(bool localAgent)
{
    if (localAgent)
    {
        std::vector<double> distanceToGoal = RVO::vectorSubstract(d->position, d->destination);

        const double sqrDistance = RVO::vectorProduct(distanceToGoal, distanceToGoal);

        if (sqrDistance > 400)
        {
            d->isFinished = false;
        }
        else
        {
            d->isFinished = true;
        }
    }

    return d->isFinished;
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
