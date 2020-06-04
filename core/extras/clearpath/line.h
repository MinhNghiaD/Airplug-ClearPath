#ifndef LINE_H
#define LINE_H

namespace ClearPath
{
    class Line {
        public:
            Line(double * point, int size_point, double * direction, int size_direction);
            double* getPosition();
            double* getDirection();
            int dimensionPoint;
            double* point;
            double* direction;
            int dimensionDirection;
    };
}
#endif // LINE_H
