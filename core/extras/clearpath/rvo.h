/* ============================================================
 *
 * This file is a part of Airplug project
 *
 * Date        : 2020-5-25
 * Description : Implement Linear programming optimization of Reciprocal Velocity Obstacles
 *
 * 2020 by Remi Besoin <remi.besoin@etu.utc.fr>
 * 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * ============================================================ */

#ifndef RVO_H
#define RVO_H

// std includes
#include <vector>

// Qt includes
#include <QVector>

namespace ClearPath
{
class Line
{
public:

    explicit Line()
    {
    }

    Line(const std::vector<double>& point, const std::vector<double>& direction)
        : point(point),
          direction(direction)
    {
        Q_ASSERT(point.size() != direction.size());
    }

    std::vector<double> point;
    std::vector<double> direction;
};


class RVO
{
public:

    static double vectorProduct(const std::vector<double>& vector1,
                                const std::vector<double>& vector2);

    static std::vector<double> vectorSubstract(const std::vector<double>& vector1,
                                               const std::vector<double>& vector2);

    /**
     * @brief determinant of matrix from 2 2D-vectors
     * @param vector1
     * @param vector2
     * @return
     */
    static double det2D(const std::vector<double>& vector1,
                        const std::vector<double>& vector2);


    static std::vector<double> scalarProduct(std::vector<double> vector, double scalar);

    /**
     * @brief Solves a two-dimensional linear program subject to linear constraints defined
     * by lines and a circular constraint.
     *
     * @param lines                Lines defining the linear constraints.
     * @param optimizationVelocity The optimization velocity.
     * @param directionOptimal     True if the direction should be optimized.
     * @return The number of the line on which it fails, or the number of lines if
     *         successful.
     */
    static int checkCollision(const std::vector<Line>&   lines,
                              double                     maxSpeed,
                              const std::vector<double>& optimizationVelocity,
                              bool                       directionOptimal,
                              std::vector<double>&       newVelocity);

    /**
     * @brief Solves a two-dimensional linear program subject to linear constraints defined
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
    static std::vector<double> collisionFreeVelocity(const std::vector<Line>& lines,
                                                     int                      beginLine,
                                                     double                   maxSpeed,
                                                     int                      numObstacleLines,
                                                     std::vector<double>&     velocity);

private:

    /**
     * @brief Solves a one-dimensional linear program on a specified line subject to linear
     * constraints defined by lines and a circular constraint.
     *
     * @param lines                Lines defining the linear constraints.
     * @param lineNo               The specified line constraint.
     * @param optimizationVelocity The optimization velocity.
     * @param directionOptimal     True if the direction should be optimized.
     * @return True if successful.
     */
    static bool avoidCollisionWithLine(const std::vector<Line>&   lines,
                                       int                        lineID,
                                       double                     maxSpeed,
                                       const std::vector<double>& optimizationVelocity,
                                       bool                       directionOptimal,
                                       std::vector<double>&       newVelocity);

private:

    static const double EPSILON;
};

}

#endif // RVO_H
