#ifndef UNSTRUCTUREDMESH_H
#define UNSTRUCTUREDMESH_H

#include "mesh.h"

class UnstructuredMesh : public Mesh {
private:
    double maximumAngle;
    double minimumEdgeLength;

public:
    UnstructuredMesh(QString &_name);
    UnstructuredMesh(QString &_name, QString &_boundaryFilePath, double &_maximumAngle, double &_minimumEdgeLength);

    void setMaximumAngle(const double &maximumAngle);
    double getMaximumAngle() const;

    void setMinimumEdgeLength(const double &minimumEdgeLength);
    double getMinimumEdgeLength() const;
};

#endif // UNSTRUCTUREDMESH_H
