#ifndef SDF_H
#define SDF_H
#pragma once
#include "implicits.h"
#include "node.h"

class SDF : public AnalyticScalarField
{
public:
    SDF() {};
    SDF(Node* _root) : root(_root) {};
    ~SDF() {};

    double Value(const Vector& point) const { return root->Value(point); };

private:
    Node* root = nullptr;
};
#endif // SDF_H
