#include "node.h"

using namespace std;

double Sphere::Value(const Vector& point) const
{
    return Norm(point - center) - radius;
}

double Cube::Value(const Vector& point) const
{
    const double x = fmax((point[0]) - center[0] - Norm(Vector(sides[0] / 2, 0, 0)),
                          center[0] - point[0] - Norm(Vector(sides[0] / 2, 0, 0)));

    const double y = fmax((point[1]) - center[1] - Norm(Vector(sides[1] / 2, 0, 0)),
                          center[1] - point[1] - Norm(Vector(sides[1] / 2, 0, 0)));

    const double z = fmax((point[2]) - center[2] - Norm(Vector(sides[2] / 2, 0, 0)),
                          center[2] - point[2] - Norm(Vector(sides[2] / 2, 0, 0)));

    return fmax(fmax(x, y), z);
}

double Cone::Value(const Vector& point) const
{
    const double q = Norm(Vector(point[0], 0, point[2]));
    return fmax(Vector(cos, sin, 0) * Vector(q, point[1], 0), -height - point[1]);
}

double Tore::Value(const Vector& point) const
{
    const double x = Norm(Vector(point[0], 0, point[2])) - largeCircleRadius;
    return Norm(Vector(x, point[1], 0)) - smallCircleRadius;
}

double Capsule::Value(const Vector& point) const
{
    const Vector capsuleDirection = extremity2 - extremity1;
    const Vector vectorFromExtremity1 = point - extremity1;

    double parameter = (capsuleDirection * vectorFromExtremity1) / (capsuleDirection * capsuleDirection);

    if (parameter < 0) { parameter = 0; }
    if (parameter > 1) { parameter = 1; }

    const Vector closestPoint = extremity1 + parameter * capsuleDirection;

    return Norm(point - closestPoint) - radius;
}

double Cylinder::Value(const Vector& point) const
{
    Vector projectedPoint(point[0], 0, point[2]);
    double distanceToCenter = Norm(projectedPoint - center);

    if (distanceToCenter > radius && (point[1] > height / 2 || point[1] < -height / 2)) {
        return fmax(distanceToCenter - radius, fabs(point[1]) - height / 2);
    }
    else if (distanceToCenter <= radius && (point[1] > height / 2 || point[1] < -height / 2)) {
        return fmax(radius - distanceToCenter, fabs(point[1]) - height / 2);
    }
    else {
        return fmax(distanceToCenter - radius, fabs(point[1]) - height / 2);
    }
}

double Translation::Value(const Vector& point) const
{
    return node->Value(point - transformation);
}

double Rotation::Value(const Vector& point) const
{
    Vector rot = point;
    double co;
    double si;

    if (transformation[0] != 0) {
        co = cos(Math::DegreeToRadian(transformation[0]));
        si = sin(Math::DegreeToRadian(transformation[0]));
        rot = Vector(rot[0], co * rot[1] - si * rot[2], si * rot[1] + co * rot[2]);
    }

    if (transformation[1] != 0) {
        co = cos(Math::DegreeToRadian(transformation[1]));
        si = sin(Math::DegreeToRadian(transformation[1]));
        rot = Vector(co * rot[0] + si * rot[2], rot[1], -si * rot[0] + co * rot[2]);
    }

    if (transformation[2] != 0) {
        co = cos(Math::DegreeToRadian(transformation[2]));
        si = sin(Math::DegreeToRadian(transformation[2]));
        rot = Vector(co * rot[0] - si * rot[1], si * rot[0] + co * rot[1], rot[2]);
    }

    return node->Value(rot);
}

double Scale::Value(const Vector& point) const
{
    Vector scaledPoint = point;
    scaledPoint *= transformation.Inverse();
    return node->Value(scaledPoint);
}

double Union::Value(const Vector& point) const
{
    return fmin(left->Value(point), right->Value(point));
}

double Intersection::Value(const Vector& point) const
{
    return fmax(left->Value(point), right->Value(point));
}

double Difference::Value(const Vector& point) const
{
    return fmax(left->Value(point), -right->Value(point));
}

double UnionSmooth::Value(const Vector& point) const {
    const double leftValue = left->Value(point);
    const double rightValue = right->Value(point);

    const double h = max(smoothingFactor - abs(leftValue - rightValue), 0.0) / smoothingFactor;

    const double gFactor = (1.0 / 6.0) * smoothingFactor * pow(h, 3.0);

    return min(leftValue, rightValue) - gFactor;
}

double IntersectionSmooth::Value(const Vector& point) const {
    const double leftValue = left->Value(point);
    const double rightValue = right->Value(point);

    const double h = max(smoothingFactor - abs(leftValue - rightValue), 0.0) / smoothingFactor;

    const double gFactor = (1.0 / 6.0) * smoothingFactor * pow(h, 3.0);

    return max(leftValue, rightValue) - gFactor;
}

double DifferenceSmooth::Value(const Vector& point) const {
    const double leftValue = left->Value(point);
    const double rightValue = -right->Value(point);

    const double h = max(smoothingFactor - abs(leftValue - rightValue), 0.0) / smoothingFactor;

    const double gFactor = (1.0 / 6.0) * smoothingFactor * pow(h, 3.0);

    return max(leftValue, rightValue) + gFactor;
}
