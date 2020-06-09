/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-24
 * Description : Board of game
 *
 * 2020 by Lucca Rawlyk
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */
#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>


// local includes
#include "constants.h"

namespace ClearPathApplication
{

class Board : public QGraphicsScene
{
public:

    Board(qreal x, qreal y, qreal width, qreal height);
    ~Board();

public:

    void addAgent(const QString& siteID);
    void addAgent(const QString& siteID, QGraphicsEllipseItem* agent);
    void updateAgentState(const QString& siteID, std::vector<double> position);

private:

    class Private;
    Private* d;
};

}
#endif // BOARD_H
