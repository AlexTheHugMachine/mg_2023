#ifndef SURFACE_H
#define SURFACE_H

#include <QVector>

#include "mesh.h"
#include "mathematics.h"
#include "bezier.h"

// surface
class Surface
{
public:
    Surface(unsigned int u, unsigned int v);
    virtual ~Surface() = default;
    virtual Mesh Polygonize() const = 0;
    void Twist(float period);

protected:
    virtual void ComputePoints() = 0;

protected:
    double u_, v_; // num cells
    std::vector<std::vector<Vector>> points_;
};

// bezier surface
class BezierSurface : public Surface
{
public:
    BezierSurface(const std::vector<std::vector<Vector>>& control_points, unsigned int u, unsigned int v);
    virtual ~BezierSurface() = default;
    virtual Mesh Polygonize() const override;

private:
    virtual void ComputePoints() override;

private:
    std::vector<std::vector<Vector>> control_points_;
};

#endif // SURFACE_H
