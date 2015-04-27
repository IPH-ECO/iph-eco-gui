#ifndef QUAD_H
#define QUAD_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

class Quad : public CGAL::Polygon_2<K> {
protected:
    double weight;

public:
    Quad() : weight(0.0) {}

    void setWeight(const double &weight) {
        this->weight = weight;
    }

    double getWeight() const {
        return weight;
    }
};

#endif // QUAD_H

