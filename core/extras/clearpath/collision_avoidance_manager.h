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
                                       KDTree* tree);

    std::vector<double> getPosition();

    std::vector<double> getVelocity();

    /**
     * @brief Add lines that are neighbors to orcaLines
     * @param KDNodes that are neighbors
     */
    void addNeighborOrcaLine(QMap<double, QVector<KDNode*>> neighbors);

    /**
     * @brief Refresh the neighbors list and update velocity
     */
    void computeNewVelocity();

    QMap<double, QVector<KDNode*>> getClosestNeighbors();

    /**
     * @brief Calculate a new velocity
     */
    void update();

    /**
     * @brief Calculate preferenceVelocity using the vectors position and destination
     */
    void setPreferenceVelocity();

    void setDestination(std::vector<double> goal);

    /**
     * @brief Indicates whether the goal has been reached
     * @return If distance to destination is superior to 400 then return false, else return true
     */
    bool reachedGoal();

    /**
     * @brief setInfo : use for updating information of distance agent
     * @param position
     * @param velocity
     * @param maxSpeed
     */
    bool setInfo(QJsonObject info);

    /**
     * @brief Get main informations about the CollisionAvoidanceManager
     * @return A QJsonObject containing position, velocity and maxspeed
     */
    QJsonObject getInfo();

    /**
     * @brief Capture the state of the CollisionAvoidanceManager
     * @return A QJsonObject containing every informations about the CollisionAvoidanceManager
     */
    QJsonObject captureState();

private:

    /**
     * @brief encodeVector
     * @param vector
     * @return
     */
    static QJsonArray encodeVector(const std::vector<double>& vector);

    /**
     * @brief decodeVector
     * @param json
     * @param vector
     * @return true if decoded sucessfully
     */
    static bool decodeVector(const QJsonArray& json, std::vector<double>& vector);


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
