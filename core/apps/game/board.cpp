#include "board.h"

// Qt includes
#include <QRectF>

#include <QVector>

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
    }

public:
    //QRectF          border;

    QVector<Agent*> players;
};


Board::Board(qreal x, qreal y, qreal width, qreal height)
    :  QGraphicsScene(x, y, width, height),
      d(new Private())
{
}

Board::~Board()
{
    delete d;
}

void Board::addPlayer(Agent* player)
{
    d->players.append(player);

    addItem(player);
}

}
