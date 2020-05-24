#include "state.h"

//qt includes
#include <QJsonDocument>
//#include <QDebug>

namespace GameApplication
{

State::State()
{
}

State::~State()
{
}

bool State::operator== (const State& s)
{
    return (left     == s.left &&
            right    == s.right &&
            up       == s.up &&
            down     == s.down &&
            xSpeed   == s.xSpeed &&
            ySpeed   == s.ySpeed &&
            x        == s.x &&
            y        == s.y &&
            frame    == s.frame);
}

QString State::toJsonString(void)
{
    QJsonObject json;
    json["left"]    = left;
    json["right"]   = right;
    json["up"]      = up;
    json["down"]    = down;
    json["x"]       = x;
    json["y"]       = y;
    json["xSpeed"]  = xSpeed;
    json["ySpeed"]  = ySpeed;
    json["frame"]   = frame;

    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

void State::loadFromJson(const QJsonObject& json)
{
    left    = json["left"].toInt();
    right   = json["right"].toInt();
    up      = json["up"].toInt();
    down    = json["down"].toInt();
    x       = json["x"].toInt();
    y       = json["y"].toInt();
    xSpeed = json["xSpeed"].toInt();
    ySpeed = json["ySpeed"].toInt();
    frame   = json["frame"].toInt();
}

}
