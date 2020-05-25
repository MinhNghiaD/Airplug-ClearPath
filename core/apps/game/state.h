/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-1
 * Description : game state class
 *
 * 2020 by Lucca Rawlyk
 *
 * ============================================================ */

#ifndef STATE_H
#define STATE_H

//qt includes
#include <QString>
#include <QJsonObject>

//local includes
#include "constants.h"

namespace GameApplication
{

class State
{
public:
    explicit State(const QString& siteID);
    State(const QJsonObject& json);

    ~State();

    bool   operator== (const State& s);

    QJsonObject toJson(void);

public:

    QString siteID;

    bool left;
    bool right;
    bool up;
    bool down;

    int xSpeed;
    int ySpeed;

    int x;
    int y;

    int frame;
};

}
#endif // STATE_H
