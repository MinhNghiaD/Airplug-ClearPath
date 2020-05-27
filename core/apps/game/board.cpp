#include "board.h"

// Qt includes
#include <QRectF>

#include <QHash>
#include <QDebug>

// local includes


namespace GameApplication
{
class Board::Private
{
public:

    Private()
    {
    }

    ~Private()
    {
        // TODO: delete agent
    }

public:

    void moveAndUpdatePlayer(Agent* agent);
    void fixCollisions(Agent* agent);
public:
    //QRectF          border;

    QHash<QString, Agent*> agents;
};


void Board::Private::moveAndUpdatePlayer(Agent* agent)
{
    agent->move();

    // TODO: broadcast self state for other
}

/*
void Board::Private::fixCollisions(Agent* agent)
{
    //TODO lazy fixing, might need to be improved
    QList<QGraphicsItem*> collidiongAgents = agent->collidingItems();

    bool collided = false;

    while(collidiongAgents.size() != 0)
    {
        collided = true;

        Agent* col = static_cast<Agent*>(collidiongAgents[0]);

        State& state = agent->getState();

        if(state.xSpeed > 0)
        {
            if(state.ySpeed > 0)
            {
                agent->setPos(state.x -1, state.y()-1);

                col->setPos(col->x()+1, col->y()+1);
            }
            else if(state.ySpeed < 0)
            {
                player.setPos(player.x()-1, player.y()+1);
                col->setPos(col->x()+1, col->y()-1);
            }
            else
            {
                player.setPos(player.x()-1, player.y());
                col->setPos(col->x()+1, col->y());
            }
        }
        else if(state.xSpeed < 0)
        {
            if(state.ySpeed > 0)
            {
                player.setPos(player.x()+1, player.y()-1);
                col->setPos(col->x()-1, col->y()+1);
            }
            else if(state.ySpeed < 0)
            {
                player.setPos(player.x()+1, player.y()+1);
                col->setPos(col->x()-1, col->y()-1);
            }
            else
            {
                player.setPos(player.x()+1, player.y());
                col->setPos(col->x()-1, col->y());
            }
        }
        else
        {
            if(state.ySpeed > 0)
            {
                player.setPos(player.x(), player.y()-1);
                col->setPos(col->x(), col->y()+1);
            }
            else
            {
                player.setPos(player.x(), player.y()+1);
                col->setPos(col->x(), col->y()-1);
            }
        }

        col->setSpeed(0,0);
        collidiongAgents = player.collidingItems();
    }

    if(collided)
    {
        player.setSpeed(0,0);
    }
}

*/


Board::Board(qreal x, qreal y, qreal width, qreal height)
    : QGraphicsScene(x, y, width, height),
      d(new Private())
{
}

Board::~Board()
{
    delete d;
}

void Board::addAgent(const QString& siteID, Agent* agent)
{
    d->agents[siteID] = agent;

    addItem(agent);
}

void Board::updateAgentState(const State& state)
{
    qDebug() << "update state for agent" << state.siteID;

    if (! d->agents.contains(state.siteID))
    {
        Agent* newAgent = new Agent(state.siteID);
        newAgent->setState(state);

        addAgent(state.siteID, newAgent);
    }
    else
    {
        d->agents[state.siteID]->setState(state);
    }
}

}
