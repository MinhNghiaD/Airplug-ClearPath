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
#include <QJsonValue>
#include <QJsonDocument>

//std includes
#include <string>

//local includes
#include "constants.h"

namespace GameApplication
{

class State
{
private:

public:
    State();
    ~State();

    State& operator=(const State& s);
    bool operator==(const State& s);

    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;

    int x_speed = 0;
    int y_speed = 0;

    int x = 0;
    int y = 0;

    int frame = 0;

    QString toJsonString(void);
    void loadFromJson(QJsonObject json_obj);
};

}
#endif // STATE_H
