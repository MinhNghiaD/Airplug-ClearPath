#include "rvo.h"
#include "line.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <stdexcept>

namespace ClearPath
{

    const double EPSILON = 0.00001;
    RVO::RVO(){}

    static double vectorProduct(double * vector1, int len_vector1, double * vector2, int len_vector2){
        if (len_vector1 != len_vector2)
            std::invalid_argument("lenght of vector1 != lenght of vector2");
        double product = 0;
        for(int i = 0; i < len_vector1; i++){
            product += vector1[i] * vector2[i];
        }
        return product;
    }

    static double* vectorSubstract(double * vector1, int len_vector1, double * vector2, int len_vector2){
        if (len_vector1 != len_vector2)
            throw std::invalid_argument("lenght of vector1 != lenght of vector2");
        double* result = (double*) malloc (sizeof (double) * len_vector1);
        for(int i = 0; i < len_vector1; i++){
            result[i] = vector1[i] - vector2[i];
        }
        return result;
    }

    static double det2D(double * vector1, int len_vector1, double * vector2, int len_vector2){
        if (len_vector1 != 2)
            throw std::invalid_argument("lenght of vector1 != 2");
        if (len_vector1 != len_vector2)
            throw std::invalid_argument("lenght of vector1 != lenght of vector2");

        return ((vector1[0] * vector2[1]) - (vector1[1] * vector2[0]));
    }

    static double* scalarProduct(double * vector1, int len_vector1, double scalar){
        double* result = (double*) malloc (sizeof (double) * len_vector1);
        for(int i = 0; i < len_vector1; i++){
            result[i] = vector1[i] * scalar;
        }
        return result;
    }

    static bool avoidCollisionWithLine(std::vector<Line> lines,int lineID, double maxSpeed,double * optimizationVelocity, int len_optimizationVelocity, bool directionOptimal, double * newVelocity){
        const double dotProduct = vectorProduct(lines.at(lineID).point, lines.at(lineID).dimensionPoint, lines.at(lineID).direction, lines.at(lineID).dimensionDirection);
        const double discriminant = pow(dotProduct, 2) + pow(maxSpeed, 2) - vectorProduct(lines.at(lineID).point, lines.at(lineID).dimensionPoint, lines.at(lineID).point, lines.at(lineID).dimensionPoint);

        if(discriminant < 0.0)
            return false;

        const double sqrtDiscriminant = sqrt(discriminant);
        double scalarLeft = -sqrtDiscriminant - dotProduct;
        double scalarRight = sqrtDiscriminant - dotProduct;

        for (int i = 0; i < lineID; i++)
        {
            const double denominator = det2D(lines.at(lineID).direction, lines.at(lineID).dimensionDirection, lines.at(i).direction, lines.at(i).dimensionDirection);
            const double numerator   = det2D(lines.at(i).direction, lines.at(i).dimensionDirection, vectorSubstract(lines.at(lineID).point, lines.at(lineID).dimensionPoint, lines.at(i).point, lines.at(i).dimensionPoint), lines.at(lineID).dimensionPoint);

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
                if (vectorProduct(optimizationVelocity, len_optimizationVelocity, lines.at(lineID).direction, lines.at(lineID).dimensionDirection) > 0.0)
                {
                    // Take right extreme.
                    for (int j = 0; j < lines.at(lineID).dimensionPoint; ++j)
                    {
                        newVelocity[j] = lines.at(lineID).point[j] + (scalarRight * lines.at(lineID).direction[j]);
                    }
                }
                else
                {
                    // Take left extreme.
                    for (int j = 0; j < lines.at(lineID).dimensionPoint; ++j)
                    {
                        newVelocity[j] = lines.at(lineID).point[j] + (scalarLeft * lines.at(lineID).direction[j]);
                    }
                }
            }
            else
            {
                // Optimize closest point.
                double scalar = vectorProduct(lines.at(lineID).direction, lines.at(lineID).dimensionDirection, vectorSubstract(optimizationVelocity, len_optimizationVelocity, lines.at(lineID).point, lines.at(lineID).dimensionPoint), lines.at(lineID).dimensionPoint);

                scalar = std::max(scalarLeft, scalar);
                scalar = std::min(scalarRight, scalar);

                for (int j = 0; j < lines.at(lineID).dimensionPoint; ++j)
                {
                    newVelocity[j] = lines.at(lineID).point[j] + (scalar * lines.at(lineID).direction[j]);
                }
            }

            return true;

    }

    static int checkCollision(std::vector<Line> lines, double maxSpeed, double * optimizationVelocity, int len_optimizationVelocity, bool directionOptimal, double * newVelocity, int len_newVelocity){
        if (directionOptimal)
        {
            // Optimize direction. Note that the optimization velocity is of unit length in
            // this case.
            for (int j = 0; j < len_newVelocity; ++j)
            {
                newVelocity[j] = optimizationVelocity[j] * maxSpeed;
            }

        }
        else if (vectorProduct(optimizationVelocity, len_optimizationVelocity, optimizationVelocity, len_optimizationVelocity) > pow(maxSpeed, 2))
        {
            // if optVelocity points to outside the circle => retrive to the closest point
            // outside the circle on the direction of optVelocity
            double norm = sqrt(vectorProduct(optimizationVelocity, len_optimizationVelocity, optimizationVelocity, len_optimizationVelocity));

            for (int j = 0; j < len_newVelocity; ++j)
            {
                newVelocity[j] = optimizationVelocity[j] * (maxSpeed / norm);
            }
        }
        else
        {
            // Optimize closest point and inside circle.
            for (int j = 0; j < len_newVelocity; ++j)
            {
                newVelocity[j] = optimizationVelocity[j];
            }
        }

        int lineID = 0;

        while (lineID < lines.size())
        {
            // Verify Linear constraint
            if (det2D(lines.at(lineID).direction, lines.at(lineID).dimensionDirection, vectorSubstract(lines.at(lineID).point, lines.at(lineID).dimensionPoint, newVelocity, len_newVelocity), lines.at(lineID).dimensionPoint) > 0.0)
            {
                // There will be collision with lineID, try to compute new velocity to avoid
                // collision

                // in Java : final Double[] tempResult = newVelocity.clone();
                double* tempResult = (double*) malloc (sizeof (double) * len_newVelocity);

                for (int i =0; i < len_newVelocity; i++){
                    tempResult[i] = newVelocity[i];
                }

                if (!avoidCollisionWithLine(lines, lineID, maxSpeed, optimizationVelocity, len_optimizationVelocity, directionOptimal,
                        newVelocity))
                {
                    // If can not find another result that satisfy an obstacle i => return the id of
                    // obstacle where it fails
                    double* newVelocity = (double*) malloc (sizeof (double) * len_newVelocity);
                    for (int i =0; i < len_newVelocity; i++){
                        newVelocity[i] = tempResult[i];
                    }
                    newVelocity = tempResult;

                    break;
                }
            }

            ++lineID;
        }

        return lineID;

    }


    static double* collisionFreeVelocity(std::vector<Line> lines, int beginLine,double maxSpeed,int numObstacleLines, double * velocity, int len_velocity)
    {
        // range of avoidance
        double distance = 0.0;

        for (int i = beginLine; i < lines.size(); ++i)
        {
            // verify constraint with obstacle i
            if (det2D(lines.at(i).direction, lines.at(i).dimensionDirection, (vectorSubstract(lines.at(i).point, lines.at(i).dimensionPoint, velocity, len_velocity)), lines.at(i).dimensionPoint) > distance)
            {
                // Result does not satisfy constraint of line i.

                // Copy lines from 0 to numObstacleLines
                std::vector<Line>::const_iterator first = lines.begin() + 0;
                std::vector<Line>::const_iterator last = lines.begin() + numObstacleLines;
                std::vector<Line> projectedLines(first, last);

                for (int j = numObstacleLines; j < i; j++)
                {
                    const double determinant = det2D(lines.at(i).direction, lines.at(i).dimensionDirection, lines.at(j).direction, lines.at(j).dimensionDirection);

                    double* point = (double*) malloc (sizeof (double) * lines.at(0).dimensionPoint);
                    //double[lines.at(0).dimensionPoint] point;

                    // Check if the object going in opposite direction
                    if (abs(determinant) <= EPSILON)
                    {
                        // Line i and line j are parallel.
                        if (vectorProduct(lines.at(i).direction, lines.at(i).dimensionDirection, lines.at(j).direction, lines.at(j).dimensionDirection) > 0.0)
                        {
                            // Line i and line j point in the same direction.
                            continue;
                        }

                        // Line i and line j point in opposite direction.
                        // get the middle of 2 points
                        for (int k = 0; k < lines.at(0).dimensionPoint; ++k)
                        {
                            point[k] = (lines.at(i).point[k] + lines.at(j).point[k]) * 0.5;
                        }
                    }
                    else
                    {
                        double scalar = ( det2D(lines.at(j).direction, lines.at(j).dimensionDirection,
                                                vectorSubstract(lines.at(i).point, lines.at(i).dimensionPoint, lines.at(j).point, lines.at(j).dimensionPoint), lines.at(i).dimensionPoint) ) / determinant;

                        for (int k = 0; k < lines.at(0).dimensionPoint; ++k)
                        {
                            point[k] = (lines.at(i).point[k] + lines.at(j).direction[k]) * scalar;
                        }
                    }

                    // final double[] direction = vectorSubstract(lines.get(j).direction, lines.get(i).direction);
                    double* direction = vectorSubstract(lines.at(j).direction, lines.at(j).dimensionDirection, lines.at(i).direction, lines.at(i).dimensionDirection);

                    double directionNorm = sqrt(vectorProduct(direction, lines.at(j).dimensionDirection, direction, lines.at(j).dimensionDirection));

                    for (int k = 0; k < lines.at(j).dimensionDirection; ++k)
                    {
                        direction[k] /= directionNorm;
                    }
                    Line line(point, lines.at(0).dimensionPoint, direction, lines.at(j).dimensionDirection);
                    projectedLines.push_back(line);
                }

                // en Java : final double[len_velocity] tempResult;
                double* tempResult = (double*) malloc (sizeof (double) * len_velocity);
                for (int i =0; i < len_velocity; i++){
                    tempResult[i] = velocity[i];
                }

                double optimizationVelocity[2] = { -lines.at(i).direction[1], lines.at(i).direction[0] };
                int len_optimizationVelocity = 2;

                // Final check if there will be any collision
                if (checkCollision(projectedLines, maxSpeed, optimizationVelocity, len_optimizationVelocity, true, velocity, len_velocity) < projectedLines
                        .size())
                {
                    // This should in principle not happen. The result is by
                    // definition already in the feasible region of this linear
                    // program. If it fails, it is due to small floating point
                    // error, and the current result is kept.
                    double* velocity = (double*) malloc (sizeof (double) * len_velocity);
                    for (int i =0; i < len_velocity; i++){
                        velocity[i] = tempResult[i];
                    }
                }

                distance = det2D(lines.at(i).direction, lines.at(i).dimensionDirection, vectorSubstract(lines.at(i).point, lines.at(i).dimensionPoint, velocity, len_velocity), lines.at(i).dimensionPoint);
            }
        }

        return velocity;
    }

}
