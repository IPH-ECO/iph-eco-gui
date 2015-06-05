#include "include/domain/structured_mesh.h"

#include <vtkPolygon.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkQuad.h>

StructuredMesh::StructuredMesh() : resolution(1) {}

StructuredMesh::StructuredMesh(QString &name) : Mesh(name), resolution(1) {}

StructuredMesh::StructuredMesh(QString &name, uint &resolution) : Mesh(name), resolution(resolution) {}

uint StructuredMesh::getResolution() const {
    return resolution;
}

void StructuredMesh::setResolution(const uint &resolution) {
    this->resolution = resolution;
}

vtkPolyData* StructuredMesh::getGrid() {
    return polyData;
}

void StructuredMesh::generate() {
    ulong bounds[4];

    computeBounds(bounds);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> quads = vtkSmartPointer<vtkCellArray>::New();
    int columns = (bounds[1] - bounds[0]) / this->resolution; // xmax - xmin
    int rows = (bounds[3] - bounds[2]) / this->resolution; // ymax - ymin
    int count = 0;
    double x = bounds[0];
    double y = bounds[2];

    points->SetNumberOfPoints(columns * rows * 4);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            double quadCenter[3] = { x + this->resolution / 2.0, y + this->resolution / 2.0, 0.0 };

            if (boundaryPolygon->pointInPolygon(quadCenter)) {
                vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
                double quadCoordinates[4][3] = {
                    { x, y, 0.0 },
                    { x + this->resolution, y, 0.0 },
                    { x + this->resolution, y + this->resolution, 0.0 },
                    { x, y + this->resolution }
                };

                for (int k = 0; k < 4; k++) {
                    quad->GetPointIds()->SetId(k, count);
                    points->SetPoint(count, quadCoordinates[k]);
                    count++;
                }

                quads->InsertNextCell(quad);
            }

            x += this->resolution;
        }
        x = bounds[0];
        y += this->resolution;
    }

    polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetPolys(quads);
}

bool StructuredMesh::isGenerated() {
    // return grid.size1() > 0;
    return true;
}

bool StructuredMesh::instanceOf(const QString &type) {
    return type.contains("StructuredMesh");
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
