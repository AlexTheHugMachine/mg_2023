#ifndef BEZIER_H
#define BEZIER_H

#include <vector>
#include <mathematics.h>

class Bezier {
public:
    Bezier(int degree, const std::vector<std::vector<Vector>>& controlPoints);

    std::vector<std::vector<Vector>> Polygonize(int numSegments) const;

private:
    int degree_;
    std::vector<std::vector<Vector>> controlPoints_;

    float Bernstein(int n, int i, float t) const;
    int Factorial(int n) const;
};

#endif // BEZIER_H
