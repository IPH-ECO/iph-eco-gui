#include "include/domain/structured_mesh.h"

StructuredMesh::StructuredMesh() : resolution(1) {}

StructuredMesh::StructuredMesh(QString &name) : Mesh(name), resolution(1) {}

StructuredMesh::StructuredMesh(QString &name, uint &resolution) : Mesh(name), resolution(resolution) {}

uint StructuredMesh::getResolution() const {
    return resolution;
}

void StructuredMesh::setResolution(const uint &resolution) {
    this->resolution = resolution;
}

matrix<Polygon>& StructuredMesh::getGrid() {
    return grid;
}

void StructuredMesh::generate() {
    std::vector<ulong> initialPoints = calculateInitialPoints();
    Point initialPoint = Point(initialPoints.at(0), initialPoints.at(2));
    int rows = (initialPoints.at(2) - initialPoints.at(3)) / this->resolution;
    int columns = (initialPoints.at(1) - initialPoints.at(0)) / this->resolution;

    grid.resize(rows, columns, false);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (isCenterInscribed(&initialPoint, i, j)) {
                Polygon quad = makeQuadFromPoint(&initialPoint, i, j);

                grid.insert_element(i, j, quad);
            }
        }
    }
}

bool StructuredMesh::isGenerated() {
    return grid.size1() > 0;
}

void StructuredMesh::buildDomain(const QString &filename) {
    grid.clear();
    Mesh::buildDomain(filename);
}

void StructuredMesh::clearGrid() {
    grid.clear();
}

void StructuredMesh::clear() {
    grid.clear();
    Mesh::clear();
}

std::vector<ulong> StructuredMesh::calculateInitialPoints() {
    MeshPolygon *boundaryPolygon = getBoundaryPolygon();
    ulong leftVertex = boundaryPolygon->left_vertex()->x();
    ulong rightVertex = boundaryPolygon->right_vertex()->x();
    ulong topVertex = boundaryPolygon->top_vertex()->y();
    ulong bottomVertex = boundaryPolygon->bottom_vertex()->y();
    std::vector<ulong> points;

    points.push_back(leftVertex - (leftVertex % this->resolution)); //A
    points.push_back(rightVertex + (rightVertex % this->resolution)); //B
    points.push_back(topVertex + (topVertex % this->resolution)); //C
    points.push_back(bottomVertex - (bottomVertex % this->resolution)); //D

    return points;
}

Polygon StructuredMesh::makeQuadFromPoint(const Point *p, const int &i, const int &j) {
    Point a(p->x() + j * this->resolution, p->y() - i * this->resolution);
    Point b(a.x() + this->resolution, a.y());
    Point c(b.x(), b.y() - this->resolution);
    Point d(c.x() - this->resolution, c.y());
    Polygon quad;

    quad.push_back(a); //A
    quad.push_back(b); //B
    quad.push_back(c); //C
    quad.push_back(d); //D

    return quad;
}

bool StructuredMesh::isCenterInscribed(const Point *p, const int &i, const int &j) {
    Point center(p->x() + j * this->resolution + this->resolution / 2, p->y() - i * this->resolution - this->resolution / 2);
    MeshPolygon *boundaryPolygon = getBoundaryPolygon();

    if (boundaryPolygon->bounded_side(center) == CGAL::ON_BOUNDED_SIDE) {
        QList<MeshPolygon*> islands = this->getIslands();

        for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
            if ((*it)->bounded_side(center) != CGAL::ON_UNBOUNDED_SIDE) {
                return false;
            }
        }

        return true;
    }

    return false;
}
