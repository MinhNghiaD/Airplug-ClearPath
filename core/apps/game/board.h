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

#include "agent.h"

namespace GameApplication
{

class Board : public QGraphicsScene
{
public:

    Board(qreal x, qreal y, qreal width, qreal height);
    ~Board();

public:

    void addAgent(const QString& siteID, Agent* agent);
    void updateAgentState(const State& state);

private:

    class Private;
    Private* d;
};

}
#endif // BOARD_H
