#include "state.h"

//Qt includes
//#include <QDebug>

//using namespace AirPlug;

namespace GameApplication
{

State::State()
{

}

State::~State()
{

}

QString State::toJsonString(void)
{
    std::string str = "{";
    str += "\"left\":";
    str += std::to_string(left);
    str += ",\"right\":";
    str += std::to_string(right);
    str += ",\"up\":";
    str += std::to_string(up);
    str += ",\"down\":";
    str += std::to_string(down);
    str += ",\"x_speed\":";
    str += std::to_string(x_speed);
    str += ",\"y_speed\":";
    str += std::to_string(y_speed);
    str += ",\"x\":";
    str += std::to_string(x);
    str += ",\"y\":";
    str += std::to_string(y);
    str += ",\"frame\":";
    str += std::to_string(frame);
    str += "}";
    return QString(str.c_str());
}

void State::loadFromJson(QJsonObject json_obj)
{
    left = json_obj["left"].toInt();
    right = json_obj["right"].toInt();
    up = json_obj["up"].toInt();
    down = json_obj["down"].toInt();
    x = json_obj["x"].toInt();
    y = json_obj["y"].toInt();
    x_speed = json_obj["x_speed"].toInt();
    y_speed = json_obj["y_speed"].toInt();
    frame = json_obj["frame"].toInt();
}

}
