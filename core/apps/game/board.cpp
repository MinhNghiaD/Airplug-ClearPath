#include "board.h"

// Qt includes
//#include <QRectF>
#include <QTimer>
#include <QHash>
#include <QPair>
#include <QDebug>

namespace ClearPathApplication
{
class Board::Private
{
public:

    Private()
        : color(2)
    {
        timer.setInterval(FRAME_PERIOD_MS);
    }

    ~Private()
    {
    }

public:

    // TODO create border and static obstacles
    const QRgb color;
    QTimer timer;

    QHash<QString, QPair<QGraphicsEllipseItem*, std::vector<double> > > agentItems;
};

Board::Board(QObject* parent)
    : QGraphicsScene(0, 0, VIEW_WIDTH, VIEW_HEIGHT, parent),
      d(new Private())
{
    connect(&(d->timer), &QTimer::timeout,
            this,        &Board::slotUpdateScene, Qt::DirectConnection);

    d->timer.start();
}

Board::~Board()
{
    clear();
    delete d;
}

void Board::addAgent(const QString& siteID)
{
    if (! d->agentItems.contains(siteID))
    {
        QGraphicsEllipseItem* newAgent = new QGraphicsEllipseItem(0, 0, AGENT_RADIUS, AGENT_RADIUS);

        QRgb groupColor =d->color + (d->agentItems.size() / NB_AGENTS) * 3;
        newAgent->setBrush(QBrush( Qt::GlobalColor(groupColor)));

        d->agentItems[siteID].first = newAgent;
    }
}

void Board::updateAgentState(const QString& siteID, std::vector<double> position)
{
    Q_ASSERT(position.size() == 2);

    if (! d->agentItems.contains(siteID))
    {
        addAgent(siteID);
    }

    d->agentItems[siteID].second = position;
}

void Board::slotUpdateScene()
{
    QList<QGraphicsItem*> addedItems = items();

    for (QHash<QString, QPair<QGraphicsEllipseItem*, std::vector<double>  > >::const_iterator iter  = d->agentItems.cbegin();
                                                                                              iter != d->agentItems.cend();
                                                                                            ++iter)
    {
        if (! addedItems.contains(iter.value().first))
        {
            addItem(iter.value().first);
        }

        iter.value().first->setPos( (VIEW_WIDTH /2.5 + iter.value().second[0]*5),
                                    (VIEW_HEIGHT/2.5 + iter.value().second[1]*5) );
    }
}

}
