/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-6-4
 * Description : Environment controller of clearpath agents
 *
 * 2020 by Mathieu Cocheteux <mathieu at cocheteux dot eu>
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>,
 *
 * ============================================================ */

#ifndef ENVIRONMENT_MANAGER_H
#define ENVIRONMENT_MANAGER_H

// Qt include
#include <QString>
#include <QJsonObject>

// local include
#include "kd_tree.h"
#include "collision_avoidance_manager.h"

namespace ClearPath
{

class EnvironmentManager
{

public:

    /**
     * @brief init: construct environment with default parameters for agents
     * @return
     */
    static EnvironmentManager* init();

    /**
     * @brief init: construct environment specified parameters for agents
     * @param timeStep
     * @param neighborDistance
     * @param maxNeighbors
     * @param timeHorizon
     * @param radius
     * @param maxSpeed
     * @param velocity
     * @return
     */
    static EnvironmentManager* init (double timeStep,
                                     double neighborDistance,
                                     int    maxNeighbors,
                                     double timeHorizon,
                                     double radius,
                                     double maxSpeed,
                                     std::vector<double> velocity);

    /**
     * @brief setInfo: update information of distance agents
     * @param name
     * @param info
     * @return
     */
    bool setInfo(const QString& name, const QJsonObject& info) const;

    /**
     * @brief setTimeStep: setter for TimeStep value of environment
     * @param period
     */
    void setTimeStep(double period);

    /**
     * @brief getInstance: returns the instance, if not initialized, creates it
     * @return
     */
    static EnvironmentManager* getInstance();

    /**
     * @brief getAgents
     * @return the agents of the tree
     */
    QMap<QString, CollisionAvoidanceManager*> getAgents() const ;

    /**
     * @brief getAgentRadius
     * @return agent radius
     */
    double getAgentRadius() const;

    /**
     * @brief doStep: updates the tree and goes to the next timestep
     */
    void doStep();

    /**
     * @brief addAgent: add agent with specified parameters to the tree
     * @param id
     * @param position
     * @param destination
     * @param velocity
     * @param timeHorizon
     * @param timeStep
     * @param maxSpeed
     * @param neighborDistance
     * @param maxNeighbors
     * @return
     */
    CollisionAvoidanceManager* addAgent(const QString& id,
                                        std::vector<double> position,
                                        std::vector<double> destination,
                                        std::vector<double> velocity,
                                        double timeHorizon,
                                        double timeStep,
                                        double maxSpeed,
                                        double neighborDistance,
                                        int    maxNeighbors);

private:

    EnvironmentManager();

    EnvironmentManager(double timeStep,
                       double neighborDistance,
                       int    maxNeighbors,
                       double timeHorizon,
                       double radius,
                       double maxSpeed,
                       std::vector<double> velocity);

private:

    double   timeStep;
    double   globalTime;
    double   neighborDistance;
    int      maxNeighbors;
    double   timeHorizon;
    double   radius;
    double   maxSpeed;
    std::vector<double> velocity;
    KDTree obstaclesTree;

    static EnvironmentManager* instance;
};

}
#endif // ENVIRONMENT_MANAGER_H
