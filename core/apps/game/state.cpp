#include "state.h"

//qt includes
//#include <QDebug>

namespace GameApplication
{

State::State(const QString& siteID)
    : siteID(siteID),
      left(false),
      right(false),
      up(false),
      down(false),
      xSpeed(0),
      ySpeed(0),
      x(0),
      y(0),
      frame(0)
{
}

State::State(const QJsonObject& json)
    : siteID(json["siteID"].toString()),
      left(json["left"].toBool()),
      right(json["right"].toBool()),
      up(json["up"].toBool()),
      down(json["down"].toBool()),
      xSpeed(json["xSpeed"].toInt()),
      ySpeed(json["ySpeed"].toInt()),
      x(json["x"].toInt()),
      y(json["y"].toInt()),
      frame(json["frame"].toInt())
{
}

State::~State()
{
}

bool State::operator== (const State& s)
{
    return (siteID   == s.siteID &&
            left     == s.left &&
            right    == s.right &&
            up       == s.up &&
            down     == s.down &&
            xSpeed   == s.xSpeed &&
            ySpeed   == s.ySpeed &&
            x        == s.x &&
            y        == s.y &&
            frame    == s.frame);
}

QJsonObject State::toJson()
{
    QJsonObject json;

    json["siteID"]  = siteID;
    json["left"]    = left;
    json["right"]   = right;
    json["up"]      = up;
    json["down"]    = down;
    json["x"]       = x;
    json["y"]       = y;
    json["xSpeed"]  = xSpeed;
    json["ySpeed"]  = ySpeed;
    json["frame"]   = frame;

    return json;
}

}
