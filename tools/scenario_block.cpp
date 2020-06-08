#include <QDebug>

#include "environment_manager.h"

using namespace ClearPath;

EnvironmentManager* setupScenario()
{
    /* Specify the default parameters for agents that are subsequently added. */
    EnvironmentManager* environment = EnvironmentManager::init(0.25, 15, 10, 5, 2, 2, {0, 0});

    /*
     * Add agents, specifying their start position, and store their goals on the
     * opposite side of the environment.
     */
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; ++j)
        {
            // Agent 1
            std::vector<double> position1 = {55 + i * 10,  55 + j * 10};
            std::vector<double> destination1 = {-75, -75};
            environment->addAgent(QLatin1String("Agent") + QString::number((i*5 + j)*4 + 0), position1, destination1);

            // Agent 2
            std::vector<double> position2 = {-55 - i * 10,  55 + j * 10};
            std::vector<double> destination2 = {75, -75};
            environment->addAgent(QLatin1String("Agent") + QString::number((i*5 + j)*4+ 1), position2, destination2);

            // Agent 3
            std::vector<double> position3 = {55 + i * 10, -55 - j * 10};
            std::vector<double> destination3 = {-75, 75};
            environment->addAgent(QLatin1String("Agent") + QString::number((i*5 + j)*4 + 2), position3, destination3);

            // Agent 4
            std::vector<double> position4 = {-55 - i * 10, -55 - j * 10};
            std::vector<double> destination4 = {75, 75};
            environment->addAgent(QLatin1String("Agent") + QString::number((i*5 + j)*4 + 3), position4, destination4);
        }
    }

    return environment;
}


bool reachedGoal(EnvironmentManager* environment)
{
    QMap<QString, CollisionAvoidanceManager*> agents = environment->getAgents();

    for (QMap<QString, CollisionAvoidanceManager*>::const_iterator iter  = agents.cbegin();
                                                                   iter != agents.cend();
                                                                 ++iter)
    {
        if (iter.value()->reachedGoal(true))
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    EnvironmentManager* environment = setupScenario();
    int nbStep = 0;

    do
    {
        environment->update();

        QMap<QString, CollisionAvoidanceManager*> agents = environment->getAgents();

        for (QMap<QString, CollisionAvoidanceManager*>::const_iterator iter  = agents.cbegin();
                                                                       iter != agents.cend();
                                                                     ++iter)
        {
            iter.value()->update();
        }

        qDebug() << "Step" << (nbStep++);
    }
    while (!reachedGoal(environment));
}

