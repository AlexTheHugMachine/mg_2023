#include "signed.h"
#include <iostream>
#include <ui_interface.h>
#include "qte.h"

using namespace std;

bool Signed::Intersect(const Ray& ray, double& distance) const
{
    // Param�tres de l'algorithme de Sphere Tracing.
    constexpr double epsilon = 0.05; // Tol�rance pour la d�tection de collision.
    constexpr int maxIterations = 100; // Nombre maximal d'it�rations.

    double t = 0.0; // Param�tre de distance le long du rayon initialis� � 0.

    for (unsigned int i = 0; i < maxIterations; ++i)
    {
        const Vector currentPos = ray.Origin() + t * ray.Direction(); // Position actuelle le long du rayon.

        const double currentValue = Value(currentPos); // Valeur de la surface implicite en ce point.

        // V�rifie si le point est suffisamment proche de la surface implicite.
        if (fabs(currentValue) < epsilon)
        {
            distance = t;
            return true; // Intersection trouv�e.
        }

        // Avance le long du rayon en fonction de la valeur de la surface implicite.
        t += currentValue;

        // Si t devient n�gatif, le rayon est sorti de l'objet.
        if (t < 0.0)
            return false;
    }

    return false; // Nombre maximal d'it�rations sans intersection => pas d'intersection.
}
