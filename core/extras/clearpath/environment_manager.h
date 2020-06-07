/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-6-4
 * Description : Environment controller of clearpath agents
 *
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>,
 * 2020 by Mathieu Cocheteux <mathieu at cocheteux dot eu>
 *
 * ============================================================ */

#ifndef ENVIRONMENT_MANAGER_H
#define ENVIRONMENT_MANAGER_H

#include <QString>
#include <QJsonObject>
#include "kd_tree.h"
#include <QVector>
#include "collision_avoidance_manager.h"


namespace ClearPath
{

    class EnvironmentManager
    {

    public:

        // This function is used for updating information of distance agents
        bool setInfo(const QString& name, const QJsonObject& info) const;


        // construct environment with default parameters for agents
        static EnvironmentManager* init();

        // construct environment specified parameters for agents
        static  EnvironmentManager* init (double timeStep,
                                          double neighborDistance,
                                          int    maxNeighbors,
                                          double timeHorizon,
                                          double radius,
                                          double maxSpeed,
                                          QVector<double> velocity);

        // setter for TimeStep value of environment
        void setTimeStep(double period);

        // returns the instance, if not initialized, creates it
        static EnvironmentManager* getInstance();

        // returns the agents of the tree
        QMap<QString, CollisionAvoidanceManager*> getAgents() const ;

        // returns agent radius
        double getAgentRadius() const;

        // updates the tree and goes to the next timestep
        void doStep();

        // add agent with specified parameters to the tree
        CollisionAvoidanceManager* addAgent(QVector<double> position,
                                            QVector<double> destination,
                                            QVector<double> velocity,
                                            double timeHorizon,
                                            double timeStep,
                                            double maxSpeed,
                                            double neighborDistance,
                                            int    maxNeighbors);


    private:
        EnvironmentManager():globalTime(0),
                             timeStep(0),
                             obstaclesTree(2){
        }

        EnvironmentManager(double timeStep,
                           double neighborDistance,
                           int    maxNeighbors,
                           double timeHorizon,
                           double radius,
                           double maxSpeed,
                           QVector<double> velocity);

        double   timeStep;
        double   globalTime;
        double   neighborDistance;
        int      maxNeighbors;
        double   timeHorizon;
        double   radius;
        double   maxSpeed;
        QVector<double> velocity;
        KDTree obstaclesTree;
        static EnvironmentManager* instance;

    };

}
#endif // ENVIRONMENT_MANAGER_H
