#ifndef RVO_H
#define RVO_H
#include <vector>
#include <line.h>

namespace ClearPath
{

class RVO
{
public:
    static const double EPSILON;
    RVO();
    static double vectorProduct(double * vector1, int len_vector1, double * vector2, int len_vector2);
    static double* vectorSubstract(double * vector1, int len_vector1, double * vector2, int len_vector2);
    static double det2D(double * vector1, int len_vector1, double * vector2, int len_vector2);
    static double* scalarProduct(double * vector1, int len_vector1, double scalar);
    static int checkCollision(std::vector<Line> lines, double maxSpeed, double * optimizationVelocity, int len_optimizationVelocity, bool directionOptimal, double * newVelocity, int len_newVelocity);
    static double* collisionFreeVelocity(std::vector<Line> lines, int beginLine,double maxSpeed,int numObstacleLines, double * velocity, int len_velocity);

private:
    static bool avoidCollisionWithLine(std::vector<Line> lines,int lineID, double maxSpeed,double * optimizationVelocity, int len_optimizationVelocity, bool directionOptimal, double * newVelocity);

};

}

#endif // RVO_H
