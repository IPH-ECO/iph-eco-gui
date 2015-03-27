#include "include/domain/unstructured_mesh.h"

#include <QFile>
#include <QStringList>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QtMath>

#include <GeographicLib/GeoCoords.hpp>

UnstructuredMesh::UnstructuredMesh() : DEFAULT_MINIMUM_ANGLE(0.125), DEFAULT_MINIMUM_EDGE_LENGTH(0.5) {}

UnstructuredMesh::UnstructuredMesh(QString &_name) :
    Mesh(_name), DEFAULT_MINIMUM_ANGLE(0.125), DEFAULT_MINIMUM_EDGE_LENGTH(0.5), minimumAngle(DEFAULT_MINIMUM_ANGLE), maximumEdgeLength(DEFAULT_MINIMUM_EDGE_LENGTH) {}

UnstructuredMesh::UnstructuredMesh(QString &_name, QString &_boundaryFilePath) : Mesh(_name, _boundaryFilePath), DEFAULT_MINIMUM_ANGLE(0.125), DEFAULT_MINIMUM_EDGE_LENGTH(0.5) {}

UnstructuredMesh::UnstructuredMesh(QString &_name, QString &_boundaryFilePath, double &_minimumAngle, double &_maximumEdgeLength) :
    Mesh(_name, _boundaryFilePath), DEFAULT_MINIMUM_ANGLE(0.125), DEFAULT_MINIMUM_EDGE_LENGTH(0.5),
    minimumAngle(qPow(sin(_minimumAngle * M_PI / 180), 2.0)), maximumEdgeLength(_maximumEdgeLength)
{}

void UnstructuredMesh::setMinimumAngle(const double &minimumAngle) {
    // http://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html
    this->minimumAngle = qPow(sin(minimumAngle * M_PI / 180), 2.0);
}

double UnstructuredMesh::getMinimumAngle() const {
    return minimumAngle;
}

double UnstructuredMesh::getMinimumAngleInDegrees() const {
    return qAsin(qSqrt(minimumAngle)) * 180 / M_PI;
}

void UnstructuredMesh::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double UnstructuredMesh::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}

double UnstructuredMesh::getUpperBoundForMaximumEdgeLength() const {
    if (getBoundaryPolygon() == NULL) {
        return 0.0;
    }

    double width = getBoundaryPolygon()->width();
    double height = getBoundaryPolygon()->height();

    return height > width ? width : height;
}

void UnstructuredMesh::generate() {
    const MeshPolygon *boundaryPolygon = getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        throw MeshException("No boundary vertices found.");
    }

    if (cdt.number_of_vertices() > 0) {
        return; // Mesh already generated
    }

    for (int i = 0; i < domain.size(); i++) {
        MeshPolygon meshPolygon = domain.at(i);
        ulong polygonVerticesCount = meshPolygon.size();

        cdt.insert(meshPolygon.vertices_begin(), meshPolygon.vertices_end());

        for (ulong j = 0; j < polygonVerticesCount; j++) {
            Point p1 = meshPolygon[j];
            Point p2 = meshPolygon[j == polygonVerticesCount - 1 ? 0 : j + 1];

            cdt.insert_constraint(p1, p2);
        }
    }

    Criteria criteria(minimumAngle, maximumEdgeLength);
    Mesher mesher(cdt, criteria);

    mesher.refine_mesh();

    mark_domains();
}

const CDT* UnstructuredMesh::getCDT() {
    return &cdt;
}

QVector<RefinementArea>& UnstructuredMesh::getRefinementPolygons() {
    return refinementAreas;
}

void UnstructuredMesh::addRefinementPolygon(QString &filename) {
    QFile refinementFile(filename);

    if (!refinementFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open refinement file. Error: %1").arg(refinementFile.errorString()));
    }

    QXmlStreamReader kml(&refinementFile);
    RefinementArea refinementArea(filename);

    while (!kml.atEnd()) {
        kml.readNext();

        if (kml.name() == "outerBoundaryIs" && kml.isStartElement()) {
            do {
                kml.readNext();
            } while (kml.name() != "coordinates" && !kml.atEnd());

            if (kml.atEnd()) {
                throw MeshException(QString("No coordinates found in domain file."));
            }

            QString coordinatesText = kml.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");
            MeshPolygon *meshPolygon = refinementArea.getMeshPolygon();

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate = GeographicLib::GeoCoords(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                Point p(utmCoordinate.Easting(), utmCoordinate.Northing());

                meshPolygon->push_back(p);
            }

            calculateOptimalEdgeLength(&refinementArea);
            refinementAreas.push_back(refinementArea);

            break;
        }
    }
}

void UnstructuredMesh::calculateOptimalEdgeLength(RefinementArea *refinementArea) {
    const MeshPolygon* meshPolygon = refinementArea == NULL ? getBoundaryPolygon() : refinementArea->getMeshPolygon();
    double width = meshPolygon->width();
    double height = meshPolygon->height();
    double smallEdgeLength = (height > width ? width : height) / 10;

    if (smallEdgeLength < DEFAULT_MINIMUM_EDGE_LENGTH) {
        if (refinementArea == NULL) {
            this->maximumEdgeLength = DEFAULT_MINIMUM_EDGE_LENGTH;
        } else {
            refinementArea->setMaximumEdgeLength(DEFAULT_MINIMUM_EDGE_LENGTH);
        }
    } else {
        if (refinementArea == NULL) {
            this->maximumEdgeLength = smallEdgeLength;
        } else {
            refinementArea->setMaximumEdgeLength(smallEdgeLength);
        }
    }
}

void UnstructuredMesh::removeRefinementArea(const QString &filename) {
    for (int i = 0; i < refinementAreas.count(); i++) {
        if (refinementAreas.at(i).getFilename() == filename) {
            refinementAreas.remove(i);
            break;
        }
    }
}

RefinementArea* UnstructuredMesh::getRefinementArea(const QString &filename) {
    for (int i = 0; i < refinementAreas.count(); i++) {
        if (refinementAreas.at(i).getFilename() == filename) {
            return const_cast<RefinementArea*>(&refinementAreas.at(i));
        }
    }

    return NULL;
}

void UnstructuredMesh::buildDomain() {
    cdt.clear();

    Mesh::buildDomain();
    calculateOptimalEdgeLength();
}

void UnstructuredMesh::clear() {
    Mesh::clear();
    cdt.clear();
    name.clear();
    boundaryFilePath.clear();
    minimumAngle = DEFAULT_MINIMUM_ANGLE;
    maximumEdgeLength = DEFAULT_MINIMUM_EDGE_LENGTH;
}

void UnstructuredMesh::mark_domains(CDT::Face_handle start, int index, QList<CDT::Edge>& border) {
    if (start->info().nesting_level != -1) {
        return;
    }

    QList<CDT::Face_handle> queue;

    queue.push_back(start);

    while (!queue.empty()) {
        CDT::Face_handle fh = queue.front();

        queue.pop_front();

        if (fh->info().nesting_level == -1) {
            fh->info().nesting_level = index;

            for (int i = 0; i < 3; i++) {
                CDT::Edge e(fh, i);
                CDT::Face_handle n = fh->neighbor(i);

                if (n->info().nesting_level == -1) {
                    if (cdt.is_constrained(e)) {
                        border.push_back(e);
                    } else {
                        queue.push_back(n);
                    }
                }
            }
        }
    }
}

void UnstructuredMesh::mark_domains() {
    for (CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); it++) {
        it->info().nesting_level = -1;
    }

    QList<CDT::Edge> border;

    mark_domains(cdt.infinite_face(), 0, border);

    while (!border.empty()) {
        CDT::Edge e = border.front();
        CDT::Face_handle n = e.first->neighbor(e.second);

        border.pop_front();

        if (n->info().nesting_level == -1) {
            mark_domains(n, e.first->info().nesting_level + 1, border);
        }
    }
}
