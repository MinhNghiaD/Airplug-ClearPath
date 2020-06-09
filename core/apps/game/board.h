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

    Board(QObject* parent = nullptr);
    ~Board();

public:

    void addAgent(const QString& siteID);
    void updateAgentState(const QString& siteID, std::vector<double> position);

private:

    Q_SLOT void slotUpdateScene();

private:

    class Private;
    Private* d;
};

}
#endif // BOARD_H
