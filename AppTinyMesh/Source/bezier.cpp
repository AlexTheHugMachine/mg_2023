#include "bezier.h"

using namespace std;

Bezier::Bezier(unsigned int numRows, unsigned int numCols)
{
    getControlPoints().resize(numRows, vector<Vector>(numCols));
}
/*
void Bezier::setControlPoint(unsigned int row, unsigned int col, const Vector& point)
{
    if (row < getControlPoints().size() && col < getControlPoints().at(0).size())
    {
        setControlPoints(getControlPoints()[row][col]) = point;
    }
}*/
