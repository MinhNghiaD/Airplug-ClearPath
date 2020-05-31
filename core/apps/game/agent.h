/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-4-24
 * Description : Agent view in the board
 *
 * 2020 by Lucca Rawlyk
 *
 * ============================================================ */

#ifndef PLAYER_H
#define PLAYER_H

//qt includes
//#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>

//local includes
#include "state.h"

namespace GameApplication
{

class Agent : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:

    /**
     * @brief Agent item is a circle in the board, each agent is attached to a site and has a radius
     * @param siteID
     * @param radius
     */
    Agent(const QString& siteID, qreal radius = AGENT_RADIUS);
    ~Agent();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    /**
     * @brief init: set agent at random position
     */
    void init();

    State& getState(void);
    int    getFrame();
    void   setState(const State& state);
    void   incrementFrame(void);

    void   move();

public:

    Q_SIGNAL void signalStateChanged();

private:

    class Private;
    Private* d;
};

}
#endif // PLAYER_H
