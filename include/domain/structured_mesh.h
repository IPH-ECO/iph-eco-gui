#ifndef STRUCTURED_MESH_H
#define STRUCTURED_MESH_H

#include <boost/numeric/ublas/matrix.hpp>

#include "include/utility/delaunay_triangulation_definitions.h"
#include "quad.h"
#include "mesh.h"

using boost::numeric::ublas::matrix;

class StructuredMesh : public Mesh {
private:
    uint resolution;
    matrix<Quad*> grid;

    std::vector<ulong> calculateInitialPoints();
    Quad* makeQuadFromPoint(const Point *p, const int &i, const int &j);
    bool isCenterInscribed(const Point *p, const int &i, const int &j);

public:
    StructuredMesh();
    StructuredMesh(QString &name);
    StructuredMesh(QString &name, uint &resolution);

    uint getResolution() const;
    void setResolution(const uint &resolution);
    matrix<Quad*>& getGrid();

    void generate();
    void clearGrid();

    virtual bool isGenerated();
    virtual void buildDomain(const QString &filename);
    virtual void clear();
};

#endif // STRUCTURED_MESH_H
