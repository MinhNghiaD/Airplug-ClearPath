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
    State();
    ~State();

    bool   operator== (const State& s);

    QString toJsonString(void);
    void loadFromJson(const QJsonObject& jsonString);

public:

    bool left  = false;
    bool right = false;
    bool up    = false;
    bool down  = false;

    int xSpeed = 0;
    int ySpeed = 0;

    int x = 0;
    int y = 0;

    int frame = 0;
};

}
#endif // STATE_H
