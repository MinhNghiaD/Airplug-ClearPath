#include "rvo.h"

#include <math.h>

namespace ClearPath
{
const double RVO::EPSILON = 0.00001;

double RVO::vectorProduct(const std::vector<double>& vector1, const std::vector<double>& vector2)
{
    return std::inner_product(vector1.begin(), vector1.end(), vector2.begin(), 0.0);
}

std::vector<double> RVO::vectorSubstract(const std::vector<double>& vector1, const std::vector<double>& vector2)
{
    Q_ASSERT(vector1.size() == vector2.size());

    std::vector<double> result;

    for (size_t i = 0; i < vector1.size(); ++i)
    {
        result.push_back(vector1[i] - vector2[i]);
    }

    return result;
}

double RVO::det2D(const std::vector<double>& vector1, const std::vector<double>& vector2)
{
    Q_ASSERT (vector1.size() == 2);
    Q_ASSERT (vector1.size() == vector2.size());

    return ((vector1[0] * vector2[1]) - (vector1[1] * vector2[0]));
}

std::vector<double> RVO::scalarProduct(std::vector<double> vector, double scalar)
{
    for (size_t i = 0; i < vector.size(); ++i)
    {
        vector[i] *= scalar;
    }

    return vector;
}


bool RVO::avoidCollisionWithLine(const std::vector<Line>&    lines,
                                 int                         lineID,
                                 double                      maxSpeed,
                                 const std::vector<double>&  optimizationVelocity,
                                 bool                        directionOptimal,
                                 std::vector<double>&        newVelocity)
{
    const double dotProduct   = vectorProduct(lines.at(lineID).point, lines.at(lineID).direction);
    const double discriminant = pow(dotProduct, 2) + pow(maxSpeed, 2) - vectorProduct(lines.at(lineID).point, lines.at(lineID).point);

    if(discriminant < 0.0)
    {
        return false;
    }

    const double sqrtDiscriminant = sqrt(discriminant);
    double scalarLeft = -sqrtDiscriminant - dotProduct;
    double scalarRight = sqrtDiscriminant - dotProduct;

    // Check if there is any collision between obstacle with lineID and other obstacles before it
    for (int i = 0; i < lineID; ++i)
    {
        const double denominator = det2D(lines.at(lineID).direction, lines.at(i).direction);
        const double numerator   = det2D(lines.at(i).direction,
                                         vectorSubstract(lines.at(lineID).point, lines.at(i).point));

        // Check if is there any objects going in opposite direction
        if (abs(denominator) <= EPSILON)
        {
            // Lines lineID and i are (almost) parallel.
            if (numerator < 0.0)
            {
                // 2 objects go in opposite direction
                return false;
            }
        }

        const double scalar = numerator / denominator;

        if (denominator >= 0.0)
        {
            // Line i bounds line lineID on the right.
            scalarRight = std::min(scalarRight, scalar);
        }
        else
        {
            // Line i bounds line lineID on the left.
            scalarLeft = std::max(scalarLeft, scalar);
        }

        if (scalarLeft > scalarRight)
        {
            return false;
        }
    }

    // Optimize direction.
    if (directionOptimal)
    {
        // if optVelocity and lines[lineID].direction on the same direction
        if (vectorProduct(optimizationVelocity, lines.at(lineID).direction) > 0.0)
        {
            // Take right extreme.
            for (int j = 0; j < lines.at(lineID).point.size(); ++j)
            {
                newVelocity[j] = lines.at(lineID).point[j] + (scalarRight * lines.at(lineID).direction[j]);
            }
        }
        else
        {
            // Take left extreme.
            for (int j = 0; j < lines.at(lineID).point.size(); ++j)
            {
                newVelocity[j] = lines.at(lineID).point[j] + (scalarLeft * lines.at(lineID).direction[j]);
            }
        }
    }
    else
    {
        // Optimize closest point.
        double scalar = vectorProduct(lines.at(lineID).direction,
                                      vectorSubstract(optimizationVelocity, lines.at(lineID).point));

        scalar = std::max(scalarLeft, scalar);
        scalar = std::min(scalarRight, scalar);

        for (int j = 0; j < lines.at(lineID).point.size(); ++j)
        {
            newVelocity[j] = lines.at(lineID).point[j] + (scalar * lines.at(lineID).direction[j]);
        }
    }

    return true;

}

int RVO::checkCollision(const std::vector<Line>&   lines,
                        double                     maxSpeed,
                        const std::vector<double>& optimizationVelocity,
                        bool                       directionOptimal,
                        std::vector<double>&       newVelocity)
{
    if (directionOptimal)
    {
        // Optimize direction. Note that the optimization velocity is of unit length in this case.
        newVelocity = scalarProduct(optimizationVelocity, maxSpeed);
    }
    else if (vectorProduct(optimizationVelocity, optimizationVelocity) > pow(maxSpeed, 2))
    {
        // if optVelocity points to outside the circle => retrive to the closest point
        // outside the circle on the direction of optVelocity
        const double norm = sqrt(vectorProduct(optimizationVelocity, optimizationVelocity));

        newVelocity = scalarProduct(optimizationVelocity, (maxSpeed / norm));
    }
    else
    {
        // Optimize closest point and inside circle.
        newVelocity = optimizationVelocity;
    }

    int lineID = 0;

    while (lineID < lines.size())
    {
        // Verify Linear constraint
        if (det2D(lines.at(lineID).direction, vectorSubstract(lines.at(lineID).point, newVelocity)) > 0.0)
        {
            // There will be collision with lineID, try to compute new velocity to avoid collision
            const std::vector<double> tempResult = newVelocity;

            if (!avoidCollisionWithLine(lines, lineID, maxSpeed, optimizationVelocity, directionOptimal, newVelocity))
            {
                // If can not find another result that satisfy an obstacle i => return the id of obstacle where it fails
                newVelocity = tempResult;

                break;
            }
        }

        ++lineID;
    }

    return lineID;

}


std::vector<double> RVO::collisionFreeVelocity(const std::vector<Line>& lines,
                                               int                      beginLine,
                                               double                   maxSpeed,
                                               int                      numObstacleLines,
                                               std::vector<double>&     velocity)
{
    // range of avoidance
    double distance = 0;

    for (int i = beginLine; i < lines.size(); ++i)
    {
        // verify constraint with obstacle i
        if (det2D(lines.at(i).direction, (vectorSubstract(lines.at(i).point, velocity))) > distance)
        {
            // Result does not satisfy constraint of line i.

            // Copy lines from 0 to numObstacleLines
            std::vector<Line>::const_iterator first = lines.begin();
            std::vector<Line>::const_iterator last = lines.begin() + numObstacleLines;
            std::vector<Line> projectedLines(first, last);

            for (int j = numObstacleLines; j < i; ++j)
            {
                const double determinant = det2D(lines.at(i).direction, lines.at(j).direction);

                std::vector<double> point = lines.at(0).point;

                // Check if the object going in opposite direction
                if (abs(determinant) <= EPSILON)
                {
                    // Line i and line j are parallel.
                    if (vectorProduct(lines.at(i).direction, lines.at(j).direction) > 0)
                    {
                        // Line i and line j point in the same direction.
                        continue;
                    }

                    // Line i and line j point in opposite direction.
                    // get the middle of 2 points
                    for (int k = 0; k < lines.at(0).point.size(); ++k)
                    {
                        point[k] = (lines.at(i).point[k] + lines.at(j).point[k]) * 0.5;
                    }
                }
                else
                {
                    double scalar = ( det2D(lines.at(j).direction,
                                            vectorSubstract(lines.at(i).point, lines.at(j).point)) ) / determinant;

                    for (int k = 0; k < lines.at(0).point.size(); ++k)
                    {
                        point[k] = (lines.at(i).point[k] + lines.at(j).direction[k]) * scalar;
                    }
                }

                std::vector<double> direction = vectorSubstract(lines.at(j).direction, lines.at(i).direction);

                double directionNorm = sqrt(vectorProduct(direction, direction));

                direction = scalarProduct(direction, 1/directionNorm);

                projectedLines.push_back(Line(point, direction));
            }

            const std::vector<double> tempResult = velocity;

            const std::vector<double> optimizationVelocity = { -lines.at(i).direction[1], lines.at(i).direction[0] };

            // Final check if there will be any collision
            if (checkCollision(projectedLines, maxSpeed, optimizationVelocity, true, velocity) < projectedLines.size())
            {
                // This should in principle not happen. The result is by
                // definition already in the feasible region of this linear
                // program. If it fails, it is due to small floating point
                // error, and the current result is kept.
                velocity = tempResult;
            }

            distance = det2D(lines.at(i).direction, vectorSubstract(lines.at(i).point, velocity));
        }
    }

    return velocity;
}

}
