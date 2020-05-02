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

// Qt include
#include <QString>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

//std include
#include <string>

// Local include
#include "constants.h"

//using namespace AirPlug;

namespace GameApplication
{

class State
{
private:

public:
    State();
    ~State();

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
