#include "include/domain/mesh_polygon.h"
#include "include/exceptions/mesh_polygon_exception.h"

#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtMath>
#include <GeographicLib/GeoCoords.hpp>
#include <vtkPoints.h>
#include <vtkMath.h>

#include <QDebug>

const QString MeshPolygon::BOUNDARY_POLYGON_FILENAME = "Main";

// To be removed
const double MeshPolygon::DEFAULT_MINIMUM_ANGLE = 20.7;

const double MeshPolygon::DEFAULT_MAXIMUM_EDGE_LENGTH = 0.5;

MeshPolygon::MeshPolygon(const QString &filename, MeshPolygonType meshPolygonType) : filename(filename), meshPolygonType(meshPolygonType) {}

void MeshPolygon::build() {
    QFile kmlFile(this->filename);

    if (!kmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshPolygonException(QString("Unable to open KML file. Error: %1").arg(kmlFile.errorString()));
    }

    QXmlStreamReader kmlReader(&kmlFile);

    originalPolygon = vtkSmartPointer<vtkPolygon>::New();

    while (!kmlReader.atEnd()) {
        kmlReader.readNext();

        if (kmlReader.name() == "outerBoundaryIs" && kmlReader.isStartElement()) {
            do {
                kmlReader.readNext();
            } while (kmlReader.name() != "coordinates" && !kmlReader.atEnd());

            if (kmlReader.atEnd()) {
                throw MeshPolygonException(QString("No coordinates found in KML file."));
            }

            QString coordinatesText = kmlReader.readElementText();
            QStringList coordinates = coordinatesText.trimmed().split(" ");
            int coordinatesCount = coordinates.size() - 1;

            originalPolygon->GetPoints()->SetNumberOfPoints(coordinatesCount);

            for (int i = 0; i < coordinatesCount; i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                double point[3] = { utmCoordinate.Easting(), utmCoordinate.Northing(), 0.0 };

                originalPolygon->GetPoints()->SetPoint(i, point);
            }

            break;
        }
    }

    kmlFile.close();
}

void MeshPolygon::filter(double distanceFilter) {
    if (originalPolygon == NULL || originalPolygon->GetPoints()->GetNumberOfPoints() == 0) {
        throw MeshPolygonException("Unable to filter polygon. Points not defined.");
    }

    if (distanceFilter == 0.0) {
        // TODO: Use shallow copy
        filteredPolygon = originalPolygon;
    } else {
        filteredPolygon = vtkSmartPointer<vtkPolygon>::New();
        double *p0 = originalPolygon->GetPoints()->GetPoint(0);

        filteredPolygon->GetPoints()->InsertNextPoint(p0);

        for (vtkIdType i = 1; i < originalPolygon->GetNumberOfPoints(); i++) {
            double *p1 = originalPolygon->GetPoints()->GetPoint(i);
            double pointsDistance = vtkMath::Distance2BetweenPoints(p0, p1);

            if (pointsDistance >= distanceFilter) {
                double x = p0[0] + (p1[0] - p0[0]) * distanceFilter / pointsDistance;
                double y = p0[1] + (p1[1] - p0[1]) * distanceFilter / pointsDistance;

                p0[0] = x;
                p0[1] = y;
                p0[2] = 0.0;
                filteredPolygon->GetPoints()->InsertNextPoint(p0);
            }
        }
    }
}

bool MeshPolygon::pointInPolygon(double *point) {
    double *dataPointer = static_cast<double*>(filteredPolygon->GetPoints()->GetData()->GetVoidPointer(0));
    int numberOfPoints = filteredPolygon->GetPoints()->GetNumberOfPoints();
    double normal[3] = { 0.0, 0.0, 1.0 };
    double bounds[6];

    filteredPolygon->GetPoints()->GetBounds(bounds);

    return vtkPolygon::PointInPolygon(point, numberOfPoints, dataPointer, bounds, normal);
}

// Refactor to return a pointer
MeshPolygon& MeshPolygon::operator=(const MeshPolygon &meshPolygon) {
    this->filename = meshPolygon.getFilename();
    this->meshPolygonType = meshPolygon.getMeshPolygonType();
    // To be removed
    this->minimumAngle = meshPolygon.getMinimumAngle();
    this->maximumEdgeLength = meshPolygon.getMaximumEdgeLength();

    return *this;
}

bool MeshPolygon::operator==(const MeshPolygon &meshPolygon) {
    return this->filename == meshPolygon.getFilename() && this->meshPolygonType == meshPolygon.getMeshPolygonType();
}

void MeshPolygon::setFilename(const QString &filename) {
    this->filename = filename;
}

QString MeshPolygon::getFilename() const {
    return filename;
}

void MeshPolygon::setMeshPolygonType(const MeshPolygonType &meshPolygonType) {
    this->meshPolygonType = meshPolygonType;
}

MeshPolygonType MeshPolygon::getMeshPolygonType() const {
    return meshPolygonType;
}

vtkPolygon* MeshPolygon::getOriginalPolygon() const {
    return originalPolygon;
}

vtkPolygon* MeshPolygon::getFilteredPolygon() const {
    return filteredPolygon;
}

// To be removed
void MeshPolygon::setMinimumAngle(const double &minimumAngle) {
    this->minimumAngle = minimumAngle;
}

double MeshPolygon::getMinimumAngle() const {
    return minimumAngle;
}

double MeshPolygon::getMinimumAngleInCGALRepresentation() const {
    // http://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html
    return qPow(sin(minimumAngle * M_PI / 180), 2.0);
}

void MeshPolygon::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double MeshPolygon::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}

// To be removed
void MeshPolygon::setOptimalParameters() {
    double *bounds = this->filteredPolygon->GetPoints()->GetBounds();
    double width = bounds[1] - bounds[0];
    double height = bounds[3] - bounds[2];
    double smallEdgeLength = std::min(width, height) / 10.0;
    double optimalEdgeLength = std::max(smallEdgeLength, DEFAULT_MAXIMUM_EDGE_LENGTH);

    this->maximumEdgeLength = optimalEdgeLength;
    this->minimumAngle = DEFAULT_MINIMUM_ANGLE;
}
