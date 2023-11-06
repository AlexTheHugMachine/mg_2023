#include "signed.h"
#include <iostream>
#include <ui_interface.h>
#include "qte.h"

using namespace std;

bool Signed::Intersect(const Ray& ray, double& distance) const
{
    constexpr double epsilon = 0.05;
    constexpr int maxIterations = 100;

    double t = 0.0;

    for (unsigned int i = 0; i < maxIterations; ++i)
    {
        const Vector currentPos = ray.Origin() + t * ray.Direction();

        const double currentValue = Value(currentPos);

        if (fabs(currentValue) < epsilon)
        {
            distance = t;
            return true;
        }

        t += currentValue;

        if (t < 0.0)
            return false;
    }

    return false;
}
