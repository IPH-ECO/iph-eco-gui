#include "include/domain/structured_mesh.h"

#include <vtkPolygon.h>
#include <vtkPoints.h>
#include <QDebug>

StructuredMesh::StructuredMesh() : resolution(1) {}

StructuredMesh::StructuredMesh(QString &name) : Mesh(name), resolution(1) {}

StructuredMesh::StructuredMesh(QString &name, uint &resolution) : Mesh(name), resolution(resolution) {}

uint StructuredMesh::getResolution() const {
    return resolution;
}

void StructuredMesh::setResolution(const uint &resolution) {
    this->resolution = resolution;
}

vtkStructuredGrid* StructuredMesh::getGrid() {
    return grid;
}

void StructuredMesh::generate() {
    ulong bounds[4];

    computeBounds(bounds);

    vtkSmartPointer<vtkPoints> gridPoints = vtkSmartPointer<vtkPoints>::New();
    QList<int> blankedPoints;
    int columns = (bounds[1] - bounds[0]) / this->resolution; // xmax - xmin
    int rows = (bounds[3] - bounds[2]) / this->resolution; // ymax - ymin
    int count = 0;
    double x = bounds[0];
    double y = bounds[2];

    gridPoints->SetNumberOfPoints(columns * rows);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            double quadCenter[3] = { x + this->resolution / 2.0, y + this->resolution / 2.0, 0.0 };

            gridPoints->SetPoint(count, x, y, 0.0);

            if (!boundaryPolygon->pointInPolygon(quadCenter)) {
                blankedPoints.append(count);
            }

            x += this->resolution;
            count++;
        }
        x = bounds[0];
        y += this->resolution;
    }

    grid = vtkSmartPointer<vtkStructuredGrid>::New();
    grid->SetDimensions(columns, rows, 1);
    grid->SetPoints(gridPoints);

    for (int i = 0; i < blankedPoints.size(); i++) {
        grid->BlankPoint(blankedPoints[i]);
    }
    grid->Modified();
}

bool StructuredMesh::isGenerated() {
    // return grid.size1() > 0;
    return true;
}

bool StructuredMesh::instanceOf(const QString &type) {
    return type.contains("StructuredMesh");
}

void StructuredMesh::buildDomain(const QString &filename) {
    // grid.clear();
    Mesh::buildDomain(filename);
}

void StructuredMesh::clearGrid() {
    // for (ulong i = 0; i < grid.size1(); i++) {
    //     for (ulong j = 0; j < grid.size2(); j++) {
    //         delete grid(i, j);
    //     }
    // }

    // grid.clear();
}

void StructuredMesh::clear() {
    clearGrid();
    Mesh::clear();
}

void StructuredMesh::computeBounds(ulong *points) {
    double *bounds = this->boundaryPolygon->getFilteredPolygon()->GetPoints()->GetBounds();

    points[0] = bounds[0] - ((ulong) bounds[0] % this->resolution); // xmin
    points[1] = bounds[1] - ((ulong) bounds[1] % this->resolution); // xmax
    points[2] = bounds[2] - ((ulong) bounds[2] % this->resolution); // ymin
    points[3] = bounds[3] - ((ulong) bounds[3] % this->resolution); // ymax
}
