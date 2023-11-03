#pragma once
#ifndef Signed_H
#include "implicits.h"
#include "node.h"

#define Signed_H
class Signed : public AnalyticScalarField
{
public:
    Signed() {};
    Signed(Node* _root) : root(_root) {};
    ~Signed() {};

    double Value(const Vector& point) const { return root->Value(point); };
    bool Intersect(const Ray& ray, double& distance) const;

private:
    Node* root = nullptr;
};

#endif // Signed_H
