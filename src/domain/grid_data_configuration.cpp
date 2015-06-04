#include "include/domain/grid_data_configuration.h"

#include <CGAL/Circle_2.h>
#include <qmath.h>

#include "include/domain/unstructured_mesh.h"
#include "include/domain/structured_mesh.h"
#include "include/exceptions/grid_data_exception.h"

GridDataConfiguration::GridDataConfiguration() : mesh(NULL), showMesh(true) {}

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

bool GridDataConfiguration::getShowMesh() const {
    return showMesh;
}

void GridDataConfiguration::setShowMesh(const bool showMesh) {
    this->showMesh = showMesh;
}

QVector<GridData*>& GridDataConfiguration::getGridDataVector() {
    return gridDataVector;
}

bool GridDataConfiguration::addGridData(GridData *gridData) {
    if (this->gridDataVector.contains(gridData)) {
        return false;
    }

    this->processGridData(gridData);
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

CellInfo* GridDataConfiguration::createCellInfoFromDataPoints(Point &centroid, GridData *gridData, QSet<GridDataPoint> &dataPoints) {
    GridInformationType gridInformationType = gridData->getGridInformationType();
    CGAL::Circle_2<K> circle(centroid, gridData->getRadius());
    QSet<GridDataPoint> tempDataPoints;

    for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
        if (circle.bounded_side(it->getPoint()) == CGAL::ON_BOUNDED_SIDE) {
            tempDataPoints.insert(*it);
        }
    }

    double weight;

    if (tempDataPoints.isEmpty()) {
        weight = calculateNearestWeight(dataPoints, centroid);
    } else {
        weight = inverseOfDistance(gridData, tempDataPoints, centroid);
    }

    return new CellInfo(gridData, gridInformationType, weight);
}

CellInfo* GridDataConfiguration::createCellInfoFromDataPolygon(Point &centroid, GridData *gridData, GridDataPolygon &dataPolygon) {
    GridInformationType gridInformationType = gridData->getGridInformationType();

    if (dataPolygon.bounded_side(centroid) == CGAL::ON_BOUNDED_SIDE) {
        double weight = dataPolygon.getData();
        return new CellInfo(gridData, gridInformationType, weight);
    }

    return NULL;
}

void GridDataConfiguration::processGridData(GridData *gridData) {
    GridInformationType gridInformationType = gridData->getGridInformationType();
    QSet<GridDataPoint> dataPoints;
    GridDataPolygon dataPolygon;
    bool isInputTypePoint = gridData->getGridInputType() == GridData::POINT;

    if (isInputTypePoint) {
        dataPoints = gridData->getGridDataPoints();
    } else {
        dataPolygon = gridData->getGridDataPolygon();
    }

    if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
        UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
        const CDT *cdt = unstructuredMesh->getCDT();

        for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
            if (!fit->info().isInDomain() || fit->info().getCellInfo(gridInformationType) != NULL) {
                continue;
            }

            CGAL::Triangle_2<K> triangle = cdt->triangle(fit);
            Point centroid = CGAL::centroid(triangle);
            CellInfo *cellInfo = NULL;

            if (isInputTypePoint) {
                cellInfo = createCellInfoFromDataPoints(centroid, gridData, dataPoints);
            } else {
                cellInfo = createCellInfoFromDataPolygon(centroid, gridData, dataPolygon);
            }

            if (cellInfo != NULL) {
                fit->info().addCellInfo(cellInfo);
            }
        }
    } else {
        StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
        // matrix<Quad*> &grid = structuredMesh->getGrid();

        // for (ulong i = 0; i < grid.size1(); i++) {
        //     for (ulong j = 0; j < grid.size2(); j++) {
        //         Quad *quad = grid(i, j);

        //         if (quad == NULL || quad->is_empty()) {
        //             continue;
        //         }

        //         Point centroid = CGAL::centroid((*quad)[0], (*quad)[1], (*quad)[2], (*quad)[3]);
        //         CellInfo *cellInfo = NULL;

        //         if (isInputTypePoint) {
        //             cellInfo = createCellInfoFromDataPoints(centroid, gridData, dataPoints);
        //         } else {
        //             cellInfo = createCellInfoFromDataPolygon(centroid, gridData, dataPolygon);
        //         }

        //         if (cellInfo != NULL) {
        //             quad->addCellInfo(cellInfo);
        //         }
        //     }
        // }
    }
}

double GridDataConfiguration::inverseOfDistance(GridData *gridData, QSet<GridDataPoint> &dataPoints, Point &centroid) {
    double numerator = 0.0, denominator = 0.0;

    for (QSet<GridDataPoint>::const_iterator it = dataPoints.begin(); it != dataPoints.end(); it++) {
        double distance = CGAL::squared_distance(centroid, it->getPoint());
        numerator += it->getData() / qPow(distance, gridData->getExponent());
        denominator += 1 / qPow(distance, gridData->getExponent());
    }

    return numerator / denominator;
}

double GridDataConfiguration::calculateNearestWeight(QSet<GridDataPoint> &dataPoints, Point &centroid) {
    QSet<GridDataPoint>::const_iterator it = dataPoints.begin();
    QSet<GridDataPoint>::const_iterator nearest = it;
    double minimalDistance = CGAL::squared_distance(centroid, nearest->getPoint());

    while (++it != dataPoints.end()) {
        double distance = CGAL::squared_distance(centroid, it->getPoint());

        if (minimalDistance > distance) {
            nearest = it;
            minimalDistance = distance;
        }
    }

    return nearest->getData();
}

QSet<CellInfo*> GridDataConfiguration::queryCells(Point &point) {
    if (dynamic_cast<UnstructuredMesh*>(mesh) != NULL) {
        UnstructuredMesh *unstructuredMesh = static_cast<UnstructuredMesh*>(mesh);
        const CDT *cdt = unstructuredMesh->getCDT();

        for (CDT::Finite_faces_iterator fit = cdt->finite_faces_begin(); fit != cdt->finite_faces_end(); ++fit) {
            if (!fit->info().isInDomain()) {
                continue;
            }

            CGAL::Triangle_2<K> triangle = cdt->triangle(fit);

            if (triangle.bounded_side(point) == CGAL::ON_BOUNDED_SIDE) {
                return fit->info().getCellInfoSet();
            }
        }
    } else {
        StructuredMesh *structuredMesh = static_cast<StructuredMesh*>(mesh);
        // matrix<Quad*> &grid = structuredMesh->getGrid();

        // for (ulong i = 0; i < grid.size1(); i++) {
        //     for (ulong j = 0; j < grid.size2(); j++) {
        //         Quad *quad = grid(i, j);

        //         if (quad == NULL || quad->is_empty()) {
        //             continue;
        //         }

        //         if (quad->bounded_side(point) == CGAL::ON_BOUNDED_SIDE) {
        //             return quad->getCellInfoSet();
        //         }
        //     }
        // }
    }

    return QSet<CellInfo*>();
}
