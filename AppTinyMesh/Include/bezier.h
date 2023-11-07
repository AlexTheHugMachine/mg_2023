#ifndef BEZIER_H
#define BEZIER_H

#include <vector>

#include "mesh.h"

class Bezier
{
public:
    static Vector DeCastledjau(const std::vector<Vector>&, double);
};

inline Vector Bezier::DeCastledjau(const std::vector<Vector>& points, double t)
{
    std::vector<Vector> current_points = points;
    unsigned int nb_points = current_points.size();
    for(unsigned int i = 0; i < points.size()-1; ++i)
    {
        for(unsigned int j = 0; j < nb_points-1; ++j)
            current_points.at(j) = (1-t)*current_points.at(j) + t*(current_points.at(j+1));
        --nb_points;
    }
    return current_points.at(0);
}

#endif // BEZIER_H
