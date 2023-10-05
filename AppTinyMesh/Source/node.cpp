#include "node.h"

/* ------------------------------------------- PRIMITIVES -------------------------------------------- */

// ========== Sph�re ==========
double Sphere::Value(const Vector& point) const
{
    return Norm(point - getCenter()) - getRadius();
    /*
    point - center => vecteur qui pointe du centre de la sph�re vers le point.
    Norm(point - center) => distance euclidienne entre le centre de la sph�re et le point.
    Norm(point - center) - radius => si > 0 cela signifie que la norme du vecteur est plus grande que le rayon donc le point est � l'ext�rieur.
    */
}

// ========== Cube ==========
double Cube::Value(const Vector& point) const
{
    const double x = fmax((point[0]) - getCenter()[0] - Norm(Vector(getSides()[0] / 2, 0, 0)),
                          getCenter()[0] - point[0] - Norm(Vector(getSides()[0] / 2, 0, 0)));

    const double y = fmax((point[1]) - getCenter()[1] - Norm(Vector(getSides()[1] / 2, 0, 0)),
                          getCenter()[1] - point[1] - Norm(Vector(getSides()[1] / 2, 0, 0)));

    const double z = fmax((point[2]) - getCenter()[2] - Norm(Vector(getSides()[2] / 2, 0, 0)),
                          getCenter()[2] - point[2] - Norm(Vector(getSides()[2] / 2, 0, 0)));

    return fmax(fmax(x, y), z);
    /*
    Norm(Vector(sides[0] / 2, 0, 0) => on prend la moiti� de la largeur du cube car on veut calculer la distance du centre jusqu'� la moiti� d'un c�t� pas jusqu'� un coin.
    Distance entre le point et le :
    * point[0]) - center[0] - Norm(Vector(sides[0] / 2, 0, 0) => c�t� droit du cube.
    * center[0]) - point[0] - Norm(Vector(sides[0] / 2, 0, 0) => c�t� gauche du cube.
    * point[1]) - center[1] - Norm(Vector(sides[1] / 2, 0, 0) => c�t� sup�rieur du cube.
    * center[1]) - point[1] - Norm(Vector(sides[1] / 2, 0, 0) => c�t� inf�rieur du cube.
    * point[2]) - center[2] - Norm(Vector(sides[2] / 2, 0, 0) => c�t� avant du cube.
    * center[2]) - point[2] - Norm(Vector(sides[2] / 2, 0, 0) => c�t� arri�re du cube.
    d positif => le point est � l'ext�rieur du cube.
    */
}

// ========== C�ne ==========
double Cone::Value(const Vector& point) const
{
    const double q = Norm(Vector(point[0], 0, point[2]));
    return fmax(Vector(getCos(), getSin(), 0) * Vector(q, point[1], 0), -getHeight() - point[1]);

    /*
    Norm(Vector(point[0], 0, point[2])) => distance du point � l'axe central du c�ne (z).
    Vector(cos, sin, 0) => distance du point au bord lat�ral du c�ne.
    Vector(q, point[1], 0) => vecteur allant du point � la base de c�ne.
    Si Vector(cos, sin, 0) * Vector(q, point[1], 0) > -height - point[1] => point � l'ext�rieur de c�ne.
    */
}

// ========== Tore ==========
double Tore::Value(const Vector& point) const
{
    const double x = Norm(Vector(point[0], 0, point[2])) - getLargeCircleRadius();
    return Norm(Vector(x, point[1], 0)) - getSmallCircleRadius();

    /*
    Vector(point[0], 0, point[2]) => projete le point sur le plan xz.
    Norm(Vector(point[0], 0, point[2])) => distance du point � l'origine.
    Norm(Vector(point[0], 0, point[2])) - getLargeCircleRadius() => distance du point au grand cercle.
    (Vector(x, point[1], 0) => vecteur du point au centre du tore.
    Norm(Vector(x, point[1], 0)) - getSmallCircleRadius() => distance du point au petit cercle.
    */
}

// ========== Capsule ==========
double Capsule::Value(const Vector& point) const
{
    const Vector capsuleDirection = getSecondExtremity() - getFirstExtremity();
    const Vector vectorFromExtremity1 = point - getFirstExtremity();

    double parameter = (capsuleDirection * vectorFromExtremity1) / (capsuleDirection * capsuleDirection);

    if (parameter < 0) { parameter = 0; }
    if (parameter > 1) { parameter = 1; }

    const Vector closestPoint = getFirstExtremity() + parameter * capsuleDirection;

    return Norm(point - closestPoint) - getRadius();
    /*
    extremity2 - extremity1 => direction de la capsule.
    point - extremity1 => vecteur de la premi�re extr�mit� au point.
    (capsuleDirection * vectorFromExtremity1) / (capsuleDirection * capsuleDirection) =>  position relative du point par rapport � la ligne de la capsule.
    0 < parameter < 1 => signifie que le point le plus proche du point sur la ligne de la capsule est � l'int�rieur de la capsule.
    parameter < 0 => point le plus proche au-del� de la premi�re extr�mit�.
    parameter > 1 => point le plus proche au-del� de la deuxi�me extr�mit� de la capsule.
    Cette �tape permet de d�terminer si le point est � l'ext�rieur de la capsule.
    extremity1 + parameter * capsuleDirection => point le plus proche sur la ligne de la capsule par rapport � point. Il le fait en ajoutant parameter fois la direction de la capsule � la premi�re extr�mit�.
    Norm(point - closestPoint) - radius => si > 0 : point � l'ext�rieur de la capsule.
                                             => si < 0 : point � l'int�rieur de la capsule.
                                             => si = 0 : point sur la surface de la capsule.
    */
}


/* ---------------------------------------- TRANSFORMATIONS ----------------------------------------- */

// ========== Translation ==========
double Translation::Value(const Vector& point) const
{
    return getNode()->Value(point - getTransformation());
    /*
    point - transformation : translation inverse du point -> ram�ne le point depuis sa position actuelle (apr�s la translation) vers sa position d'origine (avant la translation).
    Value => positif si le point est � l'ext�rieur, n�gatif s'il est � l'int�rieur.
    */
}

// ========== Rotation ==========
double Rotation::Value(const Vector& point) const
{
    Vector rot = point;
    double co;
    double si;

    if (getTransformation()[0] != 0) {
        co = cos(Math::DegreeToRadian(getTransformation()[0]));
        si = sin(Math::DegreeToRadian(getTransformation()[0]));
        rot = Vector(rot[0], co * rot[1] - si * rot[2], si * rot[1] + co * rot[2]);
    }

    if (getTransformation()[1] != 0) {
        co = cos(Math::DegreeToRadian(getTransformation()[1]));
        si = sin(Math::DegreeToRadian(getTransformation()[1]));
        rot = Vector(co * rot[0] + si * rot[2], rot[1], -si * rot[0] + co * rot[2]);
    }

    if (getTransformation()[2] != 0) {
        co = cos(Math::DegreeToRadian(getTransformation()[2]));
        si = sin(Math::DegreeToRadian(getTransformation()[2]));
        rot = Vector(co * rot[0] - si * rot[1], si * rot[0] + co * rot[1], rot[2]);
    }

    return getNode()->Value(rot);
    /*
    rot => position du point apr�s avoir subi la rotation.
    if => traite chaque composante de la rotation (rotation autour des axes X, Y et Z).
    transformation != 0 => v�rifie que la rotation a �t� appliqu�e autour de cet axe.
    co / si =>  calcul du cosinus et sinus de l'angle de rotation en degr�s (converti en radians avec Math::DegreeToRadian).
    rot => rotation inverse du vecteur rot pour ramener le point � sa position d'origine avant la rotation.
         |   1    0    0    |         |  cos   0   sin   |         |  cos  -sin  0   |
    Rx = |   0   cos  -sin  |    Ry = |   0    1    0    |    Rz = |  sin  cos   0   |
         |   0   sin  cos   |         |  -sin  0   cos   |         |   0    0    1   |
    */
}

// ========== Scale ==========
double Scale::Value(const Vector& point) const
{
    Vector scaledPoint = point;
    scaledPoint *= getTransformation().Inverse();
    return getNode()->Value(scaledPoint);
    /*
    transformation.Inverse() : inversion pour annuler l'effet de la mise � l'�chelle (si transformation = 2, l'inverse serait de 0.5 pour revenir � la taille d'origine).
    scaledPoint *= => r�duit ou agrandit chaque composant du point selon la mise � l'�chelle inverse.
    scaledPoint => position du point dans l'espace d'origine, avant la mise � l'�chelle.
    Value => positif si le point est � l'ext�rieur, n�gatif s'il est � l'int�rieur.
    */
}


/* ------------------------------------------- OPERATORS -------------------------------------------- */

// ------------------------
/* [ CLASSIC OPERATORS ] */
// ------------------------

// ========== Union ==========
double Union::Value(const Vector& point) const
{
    return fmin(getLeft()->Value(point), getRight()->Value(point));
    /*
    left->Value(point): distance sign�e entre le point et le noeud de gauche.
    right->Value(point): distance sign�e entre le point et le noeud de droite.
    fmin => plus petite distance entre le point et les deux objets, indique � quelle distance le point est du c�t� ext�rieur de l'union des deux objets.
    */
}

// ========== Intersection ==========
double Intersection::Value(const Vector& point) const
{
    return fmax(getLeft()->Value(point), getRight()->Value(point));
    /*
    left->Value(point): distance sign�e entre le point et le noeud de gauche.
    right->Value(point): distance sign�e entre le point et le noeud de droite.
    fmax => plus grande distance entre le point et les deux objets, indique � quelle distance le point est du c�t� ext�rieur de l'intersection des deux objets.
    */
}

bool Intersection::Intersect(const Ray& ray, double& distance) const
{
    // Param�tres de l'algorithme de Sphere Tracing.
    constexpr double epsilon = 0.001; // Tol�rance pour la d�tection de collision.
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


// ========== Difference ==========
double Difference::Value(const Vector& point) const
{
    return fmax(getLeft()->Value(point), -getRight()->Value(point));
    /*
    left->Value(point): distance sign�e entre le point et le noeud de gauche.
    -right->Value(point): oppos�e de la distance sign�e entre le point et le noeud de droite.
    fmax => plus grande distance entre le point et les deux objets, indique � quelle distance le point est du c�t� ext�rieur de l'intersection des deux objets.
    */
}


// -----------------------
/* [ SMOOTH OPERATORS ] */
// -----------------------

// ========== Union smooth ==========
double UnionSmooth::Value(const Vector& point) const
{
    const double smoothUnionValue = exp(-getSmoothingFactor() * getLeft()->Value(point)) + exp(-getSmoothingFactor() * getRight()->Value(point));
    return -log(fmax(0.0001, smoothUnionValue));
    /*
    smoothingfactor => facteur de lissage, utilis� pour d�terminer � quel point l'union des deux objets est lisse. Plus le facteur de lissage est �lev�, plus l'union sera lisse.
    exp => lisser les transitions entre les deux objets. Plus la valeur des objets est n�gative, plus l'exponentielle sera proche de z�ro.
    fmax =>  s'assure que smoothUnionValue n'est jamais �gal � z�ro ou n�gatif, car cela provoquerait une division par z�ro dans le calcul du logarithme.
    -log => obtient une valeur �lev�e lorsque smoothUnionValue est proche de z�ro et une valeur basse lorsque smoothUnionValue est grand.
    -log(fmax(0.0001, smoothUnionValue)) => calcule une valeur qui est �lev�e pr�s de la surface de l'objet (lorsque smoothUnionValue est proche de z�ro) et qui diminue progressivement lorsque l'on s'�loigne de la surface de l'objet.
    */
}

// ========== Intersection smooth ==========
double IntersectionSmooth::Value(const Vector& point) const
{
    const double smoothIntersectionValue = exp(getSmoothingFactor() * getLeft()->Value(point)) + exp(getSmoothingFactor() * getRight()->Value(point));
    return -log(fmax(0.0001, smoothIntersectionValue));
}

// ========== Difference smooth ==========
double DifferenceSmooth::Value(const Vector& point) const
{
    const double smoothDifferenceValue = exp(getSmoothingFactor() * getLeft()->Value(point)) + exp(-getSmoothingFactor() * getRight()->Value(point));
    return -log(fmax(0.0001, smoothDifferenceValue));
}
