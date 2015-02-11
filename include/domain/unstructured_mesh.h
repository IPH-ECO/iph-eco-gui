#ifndef UNSTRUCTUREDMESH_H
#define UNSTRUCTUREDMESH_H

#include "mesh.h"

class UnstructuredMesh : public Mesh {
private:
    double minimumAngle;
    double maximumEdgeLength;

public:
    UnstructuredMesh(QString &_name);
    UnstructuredMesh(QString &_name, QString &_boundaryFilePath, double &_minimumAngle, double &_maximumEdgeLength);

    void setMinimumAngle(const double &minimumAngle);
    double getMinimumAngle() const;

    void setMaximumEdgeLength(const double &maximumEdgeLength);
    double getMaximumEdgeLength() const;
};

#endif // UNSTRUCTUREDMESH_H
