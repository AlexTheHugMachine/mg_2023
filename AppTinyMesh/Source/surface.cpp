#include "surface.h"
#include <iostream>

// surface
Surface::Surface(unsigned int u, unsigned int v) : u_(1.0/(double)(u)), v_(1.0/(double)(v))
{}

void Surface::Twist(float period)
{
    for(unsigned int j = 0; j < points_.size(); ++j)
    {
        for(unsigned int i = 0; i < points_.at(j).size(); ++i)
        {
            float theta = (2 * 3.14 * points_.at(j).at(i)[0]) / period;
            float x = points_.at(j).at(i)[0];
            float y = points_.at(j).at(i)[1] * cos(theta) - points_.at(j).at(i)[2] * sin(theta);
            float z = points_.at(j).at(i)[1] * sin(theta) + points_.at(j).at(i)[2] + cos(theta);
            points_.at(j).at(i) = Vector(x, y, z);
        }
    }
}

// bezier surface
BezierSurface::BezierSurface(const std::vector<std::vector<Vector> > &control_points, unsigned int u, unsigned int v)
    : Surface(u, v), control_points_(control_points)
{
    ComputePoints();
}

Mesh BezierSurface::Polygonize() const
{
    std::vector<Vector> vertices;
    std::vector<Vector> normals;
    std::vector<int> vertex_indices;
    std::vector<int> normal_indices;

    for(unsigned int i = 0; i < points_.size(); ++i)
    {
        for(unsigned int j = 0; j < points_.at(i).size(); ++j)
        {
            vertices.push_back(points_.at(i).at(j));
            normals.push_back(Vector(0.0, 1.0, 0.0));
            if(i < points_.size()-1 && j < points_.at(i).size()-1)
            {
                vertex_indices.push_back(i*points_.at(i).size()+j);
                vertex_indices.push_back((i+1)*points_.at(i).size()+j);
                vertex_indices.push_back((i+1)*points_.at(i).size()+j+1);

                vertex_indices.push_back(i*points_.at(i).size()+j);
                vertex_indices.push_back((i+1)*points_.at(i).size()+j+1);
                vertex_indices.push_back(i*points_.at(i).size()+j+1);

                normal_indices.push_back(i*points_.at(i).size()+j);
                normal_indices.push_back((i+1)*points_.at(i).size()+j);
                normal_indices.push_back((i+1)*points_.at(i).size()+j+1);

                normal_indices.push_back(i*points_.at(i).size()+j);
                normal_indices.push_back((i+1)*points_.at(i).size()+j+1);
                normal_indices.push_back(i*points_.at(i).size()+j+1);
            }
        }
    }

    Mesh mesh(vertices, normals, vertex_indices, normal_indices);
    mesh.SmoothNormals();
    return mesh;
}

void BezierSurface::ComputePoints()
{
    double t = 0.0;
    double j = 0.0;

    while(t < 1.0)
    {
        j = 0.0;
        std::vector<Vector> intermediate_points;
        intermediate_points.reserve(control_points_.size());

        for(unsigned int i = 0; i < control_points_.size(); ++i)
            intermediate_points.push_back(Bezier::DeCastledjau(control_points_.at(i), t));

        std::vector<Vector> row_points;
        while(j < 1.0)
        {
            row_points.push_back(Bezier::DeCastledjau(intermediate_points, j));
            j += v_;
        }

        points_.push_back(row_points);
        t += u_;
    }
}
