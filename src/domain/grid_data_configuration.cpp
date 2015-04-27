#include "include/domain/grid_data_configuration.h"

#include <CGAL/Circle_2.h>
#include <qmath.h>

#include "include/domain/unstructured_mesh.h"
#include "include/domain/structured_mesh.h"

GridDataConfiguration::GridDataConfiguration() {}

GridDataConfiguration::~GridDataConfiguration() {
    for (int i = 0; i < gridDataVector.size(); i++) {
        delete gridDataVector.at(i);
    }
}

QString GridDataConfiguration::getName() const {
    return this->name;
}

void GridDataConfiguration::setName(const QString &name) {
    this->name = name;
}

Mesh* GridDataConfiguration::getMesh() const {
    return mesh;
}

void GridDataConfiguration::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

QVector<GridData*>& GridDataConfiguration::getGridDataVector() {
    return gridDataVector;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    if (this->gridDataVector.contains(gridData)) {
        return false;
    }

    this->gridDataVector.push_back(gridData);
    return true;
}

void GridDataConfiguration::removeGridData(int i) {
    GridData *gridData = NULL;

    if (i < gridDataVector.count()) {
        gridData = gridDataVector.at(i);
        gridDataVector.remove(i);
    }

    delete gridData;
}

GridData* GridDataConfiguration::getGridData(int i) {
    if (i < gridDataVector.count()) {
        return gridDataVector.at(i);
    }

    return NULL;
}

void GridDataConfiguration::processGridData(GridData *gridData) {
    if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
        UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
        const CDT *cdt = unstructuredMesh->getCDT();

        for (CDT::Finite_faces_iterator cell = cdt->finite_faces_begin(); cell != cdt->finite_faces_end(); ++cell) {
            if (!cell->info().inDomain()) {
                continue;
            }

            CGAL::Triangle_2<K> triangle = cdt->triangle(cell);
            Point centroid = CGAL::centroid(triangle);

            if (gridData->getGridInputType() == GridData::POINT) {
                CGAL::Circle_2<K> circle(centroid, gridData->getRadius());
                QSet<GridDataPoint> &gridDataPoints = gridData->getGridDataPoints();

                for (QSet<GridDataPoint>::const_iterator it = gridDataPoints.begin(); it != gridDataPoints.end(); it++) {
                    if (circle.bounded_side(it->getPoint()) != CGAL::ON_BOUNDED_SIDE) {
                        gridDataPoints.remove(*it);
                    }
                }

                cell->info().weight = inverseOfDistance(gridData, gridDataPoints, centroid);
            } else {
                GridDataPolygon dataPolygon = gridData->getGridDataPolygon();

                if (dataPolygon.bounded_side(centroid) == CGAL::ON_BOUNDED_SIDE) {
                    cell->info().weight = dataPolygon.getData();
                }
            }
        }
    } else {
        StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
        matrix<Quad> &grid = structuredMesh->getGrid();

        for (ulong i = 0; i < grid.size1(); i++) {
            for (ulong j = 0; j < grid.size2(); j++) {
                Quad &cell = grid(i, j);
                Point centroid = CGAL::centroid(cell.vertex(0), cell.vertex(1), cell.vertex(2), cell.vertex(3));

                if (gridData->getGridInputType() == GridData::POINT) {
                    CGAL::Circle_2<K> circle(centroid, gridData->getRadius());
                    QSet<GridDataPoint> &gridDataPoints = gridData->getGridDataPoints();

                    for (QSet<GridDataPoint>::const_iterator it = gridDataPoints.begin(); it != gridDataPoints.end(); it++) {
                        if (circle.bounded_side(it->getPoint()) != CGAL::ON_BOUNDED_SIDE) {
                            gridDataPoints.remove(*it);
                        }
                    }

                    cell.setWeight(inverseOfDistance(gridData, gridDataPoints, centroid));
                } else {
                    GridDataPolygon dataPolygon = gridData->getGridDataPolygon();

                    if (dataPolygon.bounded_side(centroid) == CGAL::ON_BOUNDED_SIDE) {
                        cell.setWeight(dataPolygon.getData());
                    }
                }
            }
        }
    }
}

double GridDataConfiguration::inverseOfDistance(GridData *gridData, QSet<GridDataPoint> &dataPoints, Point &centroid) {
    double numerator = 0, denominator = 0;

    for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
        double distance = CGAL::squared_distance(centroid, it->getPoint());
        numerator += it->getData() / qPow(distance, gridData->getExponent());
        denominator += 1 / qPow(distance, gridData->getExponent());
    }

    return numerator / denominator;
}
