#ifndef RVO_H
#define RVO_H

// std includes
#include <vector>

// Qt includes
#include <QVector>

// Local include
#include "line.h"

namespace ClearPath
{

class RVO
{
public:
    RVO();

    static double vectorProduct(const std::vector<double>& vector1, const std::vector<double>& vector2)
    {
        return std::inner_product(vector1.begin(), vector1.end(), vector2.begin(), 0.0);
    }

    static std::vector<double> vectorSubstract(const std::vector<double>& vector1, const std::vector<double>& vector2)
    {
        Q_ASSERT(vector1.size() == vector2.size());

        std::vector<double> result;

        for (size_t i = 0; i < vector1.size(); ++i)
        {
            result.push_back(vector1[i] - vector2[i]);
        }

        return result;
    }

    /**
     *
     * @param vector1
     * @param vector2
     * @return determinant of matrix from 2 2D vectors
     */
    static double det2D(const std::vector<double>& vector1, const std::vector<double>& vector2)
    {
        Q_ASSERT (vector1.size() == 2);
        Q_ASSERT (vector1.size() == vector2.size());

        return ((vector1[0] * vector2[1]) - (vector1[1] * vector2[0]));
    }


    static std::vector<double> scalarProduct(std::vector<double> vector, double scalar)
    {
        for (size_t i = 0; i < vector.size(); ++i)
        {
            vector[i] *= scalar;
        }

        return vector;
    }

    /**
     * Solves a one-dimensional linear program on a specified line subject to linear
     * constraints defined by lines and a circular constraint.
     *
     * @param lines                Lines defining the linear constraints.
     * @param lineNo               The specified line constraint.
     * @param optimizationVelocity The optimization velocity.
     * @param directionOptimal     True if the direction should be optimized.
     * @return True if successful.
     */
    static bool avoidCollisionWithLine(QVector<Line> lines,
                                       int lineID,
                                       double maxSpeed,
                                       std::vector<double> optimizationVelocity,
                                       bool directionOptimal,
                                       std::vector<double>& newVelocity)
    {
        // Prototype
        return true;
    }

    /**
     * Solves a two-dimensional linear program subject to linear constraints defined
     * by lines and a circular constraint.
     *
     * @param lines                Lines defining the linear constraints.
     * @param optimizationVelocity The optimization velocity.
     * @param directionOptimal     True if the direction should be optimized.
     * @return The number of the line on which it fails, or the number of lines if
     *         successful.
     */
    static int checkCollision(QVector<Line> lines,
                              double    maxSpeed,
                              std::vector<double>  optimizationVelocity,
                              bool   directionOptimal,
                              std::vector<double>& newVelocity)
    {
        // Prototype
        return lines.size();
    }

    /**
     * Solves a two-dimensional linear program subject to linear constraints defined
     * by lines and a circular constraint.
     *
     * @param numObstacleLines Count of obstacle lines.
     * @param beginLine        The line on which the 2-D linear program failed.
     * @param maxSpeed         Limit of speed.
     * @param numObstacleLines Number of obstacles
     * @param velocity         Current velocity to optimize
     *
     * @return new velocity
     */
    static std::vector<double> collisionFreeVelocity(QVector<Line> lines,
                                                     int      beginLine,
                                                     double   maxSpeed,
                                                     int      numObstacleLines,
                                                     std::vector<double>& velocity)
    {
        // Prototype
        return velocity;
    }

private:

    static const double EPSILON;
};

}

#endif // RVO_H
