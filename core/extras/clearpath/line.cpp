#include "line.h"
#include <stdexcept>

namespace ClearPath
{
    Line::Line(double * point, int size_point, double * direction, int size_direction){
        if (size_point != size_direction)
            throw std::invalid_argument("lenght of point != lenght of direction");
        dimensionPoint = size_point;
        point = point;
        direction = direction;
        dimensionDirection = size_direction;
    }

    double* Line::getPosition(){
        return point;
    }

    double* Line::getDirection(){
        return direction;
    }

}
