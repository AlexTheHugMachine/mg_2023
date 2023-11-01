#include "bezier.h"
#include <iostream>
#include <cmath>

Bezier::Bezier(int degree, const std::vector<std::vector<Vector>>& controlPoints)
    : degree_(degree), controlPoints_(controlPoints) {}

std::vector<std::vector<Vector>> Bezier::Polygonize(int numSegments) const {
    std::vector<std::vector<Vector>> mesh;

    if (degree_ < 1 || controlPoints_.empty() || controlPoints_[0].empty()) {
        std::cerr << "Le patch Bezier est mal défini." << std::endl;
            return mesh;
    }

    int numRows = controlPoints_.size();
    int numCols = controlPoints_[0].size();

    for (int i = 0; i < numSegments; i++) {
        float t = static_cast<float>(i) / static_cast<float>(numSegments - 1);
        std::vector<Vector> row;
        for (int j = 0; j < numSegments; j++) {
            float s = static_cast<float>(j) / static_cast<float>(numSegments - 1);
            Vector point(0.0f, 0.0f, 0.0f);

            for (int rowIdx = 0; rowIdx < numRows; rowIdx++) {
                for (int colIdx = 0; colIdx < numCols; colIdx++) {
                    float bernsteinValue = Bernstein(degree_, rowIdx, t) *
                                           Bernstein(degree_, colIdx, s);
                    //point.X += bernsteinValue * controlPoints_[rowIdx][colIdx].X;
                    //point.Y += bernsteinValue * controlPoints_[rowIdx][colIdx].Y;
                    //point.Z += bernsteinValue * controlPoints_[rowIdx][colIdx].Z;
                }
            }
            row.push_back(point);
        }
        mesh.push_back(row);
    }

    return mesh;
}

float Bezier::Bernstein(int n, int i, float t) const {
    // Calcule le coefficient de Bernstein B(i, n)(t) pour un point de contrôle
    return static_cast<float>(Factorial(n) / (Factorial(i) * Factorial(n - i))) *
           std::pow(1 - t, n - i) * std::pow(t, i);
}

int Bezier::Factorial(int n) const {
    if (n <= 1) return 1;
    return n * Factorial(n - 1);
}

