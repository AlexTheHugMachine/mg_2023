#ifndef BEZIER_H
#define BEZIER_H

#include <vector>
#include <iostream>

#include "mesh.h"

using namespace std;

class Bezier {

public:
    Bezier(unsigned int numRows, unsigned int numCols);

    vector<vector<Vector>> getControlPoints() { return controlPoints; };
    void setControlPoints(vector<vector<Vector>> _controlPoints) {
        controlPoints = _controlPoints;
    };

    void setControlPoint(unsigned int row, unsigned int col, const Vector& point);

private:
    vector<vector<Vector>> controlPoints;
};

#endif // BEZIER_H
