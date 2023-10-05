#ifndef NODE_H
#define NODE_H

#include <vector>
#include "mathematics.h"
#include <ray.h>

/* ------------------------------------------ THE MAIN CLASSES ------------------------------------------ */

// ========== Node ==========
class Node
{
public:
    Node() {};
    ~Node() {};

    virtual double Value(const Vector& point) const = 0;
};

// ========== Signed ==========
class Signed
{
public:
    Signed() {};
    ~Signed() {};

    virtual bool Intersect(const Ray& ray, double& distance) const = 0;
};


/* ------------------------------------------- PRIMITIVES -------------------------------------------- */

// ========== Sph�re ==========
class Sphere : public Node
{
public:
    Sphere() : center(Vector(0,0,0)), radius(0.0) {};
    Sphere(const Vector& _center, const double& _radius) : center(_center), radius(_radius) {};
    ~Sphere() {};

    Vector getCenter() const { return center; }
    double getRadius() const { return radius; }

    double Value(const Vector& point) const override;

private:
    Vector center;
    double radius;
};

// ========== Cube ==========
class Cube : public Node
{
public:

    Cube() : center(Vector(0, 0, 0)), sides(Vector(0, 0, 0)) {};
    Cube(const Vector& _center, const Vector& _sides) : center(_center), sides(_sides) {};
    ~Cube() {};

    Vector getCenter() const { return center; }
    Vector getSides() const { return sides; }

    double Value(const Vector& point) const override;

private:
    Vector center;
    Vector sides;
};

// ========== C�ne ==========
class Cone : public Node
{
public:
    Cone() : sin(0.0), cos(0.0), height(0.0) {};
    Cone(const double& _sin, const double& _cos, const double& _height) : sin(_sin), cos(_cos), height(_height) {};
    ~Cone() {};

    double getSin() const { return sin; }
    double getCos() const { return cos; }
    double getHeight() const { return height; }

    double Value(const Vector& point) const override;

private:
    double sin;
    double cos;
    double height;
};

// ========== Tore ==========
class Tore : public Node
{
public:
    Tore() : smallCircleRadius(0.0), largeCircleRadius(0.0) {};
    Tore(const double& _smallCircleRadius, const double& _largeCircleRadius) : smallCircleRadius(_smallCircleRadius), largeCircleRadius(_largeCircleRadius) {};
    ~Tore() {};

    double getSmallCircleRadius() const { return smallCircleRadius; }
    double getLargeCircleRadius() const { return largeCircleRadius; }

    double Value(const Vector& point) const override;

private:
    double smallCircleRadius;
    double largeCircleRadius;
};

// ========== Capsule ==========
class Capsule : public Node
{
public:
    Capsule() : extremity1(Vector(0, 0, 0)), extremity2(Vector(0, 0, 0)), radius(0.0) {};
    Capsule(const Vector& _extremity1, const Vector& _extremity2, const double& _radius) : extremity1(_extremity1), extremity2(_extremity2), radius(_radius) {};
    ~Capsule() {};

    Vector getFirstExtremity() const { return extremity1; }
    Vector getSecondExtremity() const { return extremity2; }
    double getRadius() const { return radius; }

    double Value(const Vector& point) const override;

private:
    Vector extremity1;
    Vector extremity2;
    double radius;
};


/* ---------------------------------------- TRANSFORMATIONS ----------------------------------------- */

// ========== Translation ==========
class Translation : public Node
{
public:
    Translation() : node(nullptr), transformation(Vector(0, 0, 0)) {};
    Translation(Node* _node, const Vector& _transformation) : node(_node), transformation(_transformation) {};
    ~Translation()
    {
        delete node;
    };

    Node* getNode() const { return node; }
    Vector getTransformation() const { return transformation; }

    double Value(const Vector& point) const override;

private:
    Node* node;
    Vector transformation;
};

// ========== Rotation ==========
class Rotation : public Node
{
public:
    Rotation() : node(nullptr), transformation(Vector(0, 0, 0)) {};
    Rotation(Node* _node, const Vector& _transformation) : node(_node), transformation(_transformation) {};
    ~Rotation()
    {
        delete node;
    };

    Node* getNode() const { return node; }
    Vector getTransformation() const { return transformation; }

    double Value(const Vector& point) const override;

private:
    Node* node;
    Vector transformation;
};

// ========== Scale ==========
class Scale : public Node
{
public:
    Scale() : node(nullptr), transformation(Vector(0, 0, 0)) {};
    Scale(Node* _node, const Vector& _transformation) : node(_node), transformation(_transformation) {};
    ~Scale()
    {
        delete node;
    };

    Vector getTransformation() const { return transformation; }
    Node* getNode() const {	return node; }

    double Value(const Vector& point) const override;

private:
    Node* node;
    Vector transformation;
};


/* ------------------------------------------- OPERATORS -------------------------------------------- */

// ------------------------
/* [ CLASSIC OPERATORS ] */
// ------------------------

// ========== Union ==========
class Union : public Node
{
public:
    Union() : left(nullptr), right(nullptr) {};
    Union(Node* _left, Node* _right) : left(_left), right(_right) {};
    ~Union() {
        delete left;
        delete right;
    }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }

    double Value(const Vector& point) const override;

private:
    Node* left;
    Node* right;
};

// ========== Intersection ==========
class Intersection : public Node, public Signed
{
public:
    Intersection() : left(nullptr), right(nullptr) {};
    Intersection(Node* _left, Node* _right) : left(_left), right(_right) {};
    ~Intersection() {
        delete left;
        delete right;
    }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }

    double Value(const Vector& point) const override;
    bool Intersect(const Ray& ray, double& distance) const override;

private:
    Node* left;
    Node* right;
};

// ========== Difference ==========
class Difference : public Node
{
public:
    Difference() : left(nullptr), right(nullptr) {};
    Difference(Node* _left, Node* _right) : left(_left), right(_right) {};
    ~Difference() {
        delete left;
        delete right;
    }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }

    double Value(const Vector& point) const override;

private:
    Node* left;
    Node* right;
};


// -----------------------
/* [ SMOOTH OPERATORS ] */
// -----------------------

// ========== Union smooth ==========
class UnionSmooth : public Node
{
public:
    UnionSmooth(Node* _left, Node* _right, const double &_smoothingFactor) : left(_left), right(_right), smoothingFactor(_smoothingFactor){};
    ~UnionSmooth() {
        delete left;
        delete right;
    }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    double getSmoothingFactor() const { return smoothingFactor; }

    double Value(const Vector& point) const override;

private:
    Node* left;
    Node* right;
    double smoothingFactor;
};

// ========== Intersection smooth ==========
class IntersectionSmooth : public Node
{
public:
    IntersectionSmooth(Node* _left, Node* _right, const double& _smoothingFactor) : left(_left), right(_right), smoothingFactor(_smoothingFactor) {};
    ~IntersectionSmooth() {
        delete left;
        delete right;
    }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    double getSmoothingFactor() const { return smoothingFactor; }

    double Value(const Vector& point) const override;

private:
    Node* left;
    Node* right;
    double smoothingFactor;
};

// ========== Difference smooth ==========
class DifferenceSmooth : public Node
{
public:
    DifferenceSmooth(Node* _left, Node* _right, const double& _smoothingFactor) : left(_left), right(_right), smoothingFactor(_smoothingFactor) {};
    ~DifferenceSmooth() {
        delete left;
        delete right;
    }

    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }
    double getSmoothingFactor() const { return smoothingFactor; }

    double Value(const Vector& point) const override;

private:
    Node* left;
    Node* right;
    double smoothingFactor;
};

#endif // NODE_H
