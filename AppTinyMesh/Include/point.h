#ifndef POINT_H
#define POINT_H

struct Point {

public:
    Point() : x(0.0), y(0.0), z(0.0) {};
    Point(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};

protected:
    double x;
    double y;
    double z;
};

#endif
