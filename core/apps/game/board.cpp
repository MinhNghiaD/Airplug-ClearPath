#include "board.h"

// Qt includes
//#include <QRectF>

#include <QHash>
#include <QDebug>

namespace ClearPathApplication
{
class Board::Private
{
public:

    Private()
        : color(5)
    {
    }

    ~Private()
    {
        QHash<QString, QGraphicsEllipseItem*>::iterator iter = agentItems.begin();

        while (iter != agentItems.end())
        {
            delete iter.value();
            iter = agentItems.erase(iter);
        }
    }

public:

    // TODO create border and static obstacles
    //QRectF          border;

    QHash<QString, QGraphicsEllipseItem*> agentItems;
    QRgb color;
};

Board::Board()
    : QGraphicsScene(0, 0, VIEW_WIDTH, VIEW_HEIGHT),
      d(new Private())
{
}

Board::~Board()
{
    delete d;
}

void Board::addAgent(const QString& siteID)
{
    if (! d->agentItems.contains(siteID))
    {
        QGraphicsEllipseItem* newAgent = new QGraphicsEllipseItem(0, 0, AGENT_RADIUS, AGENT_RADIUS);
        //newAgent->setFlag(QGraphicsItem::ItemIsFocusable, true);
        newAgent->setBrush(QBrush( Qt::GlobalColor(d->color) ));
        d->color += 5;

        addAgent(siteID, newAgent);
    }
}

void Board::addAgent(const QString& siteID, QGraphicsEllipseItem* agent)
{
    d->agentItems[siteID] = agent;

    addItem(agent);
}

void Board::updateAgentState(const QString& siteID, std::vector<double> position)
{
    Q_ASSERT(position.size() == 2);

    qDebug() << "update state for agent" << siteID;

    if (! d->agentItems.contains(siteID))
    {
        addAgent(siteID);
    }

    d->agentItems[siteID]->setPos(position[0], position[1]);
}

}
