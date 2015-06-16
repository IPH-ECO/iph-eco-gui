#include "include/domain/mesh_polygon.h"
#include "include/exceptions/mesh_polygon_exception.h"

#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <GeographicLib/GeoCoords.hpp>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>

const QString MeshPolygon::BOUNDARY_POLYGON_NAME = "Boundary";

const double MeshPolygon::DEFAULT_MINIMUM_ANGLE = 20.7;

const double MeshPolygon::DEFAULT_MAXIMUM_EDGE_LENGTH = 0.5;

MeshPolygon::MeshPolygon() : id(0) {}

MeshPolygon::MeshPolygon(const QString &name, const QString &filename, const MeshPolygonType &meshPolygonType) :
    id(0), name(name), meshPolygonType(meshPolygonType), filename(filename) {}

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
            int coordinatesCount = coordinates.size() - 1; // KML repeats the first coordinate at the end of the list

            originalPolygon->GetPoints()->SetNumberOfPoints(coordinatesCount);
            originalPolygon->GetPointIds()->SetNumberOfIds(coordinatesCount);

            for (int i = 0; i < coordinatesCount; i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords utmCoordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());
                double point[3] = { utmCoordinate.Easting(), utmCoordinate.Northing(), 0.0 };

                originalPolygon->GetPoints()->SetPoint(i, point);
                originalPolygon->GetPointIds()->SetId(i, i);
            }

            break;
        }
    }

    kmlFile.close();
}

void MeshPolygon::filter(double &distanceFilter) {
    if (originalPolygon == NULL || originalPolygon->GetPoints()->GetNumberOfPoints() == 0) {
        throw MeshPolygonException("Unable to filter polygon. Points not defined.");
    }

    if (distanceFilter == 0.0) {
        filteredPolygon = originalPolygon;
    } else {
        filteredPolygon = vtkSmartPointer<vtkPolygon>::New();
        double p1[3];
        int count = 1;
        vtkIdType i = 1;

        originalPolygon->GetPoints()->GetPoint(0, p1);
        filteredPolygon->GetPoints()->InsertPoint(0, p1);
        filteredPolygon->GetPointIds()->InsertId(0, 0);

        while (i < originalPolygon->GetPoints()->GetNumberOfPoints()) {
            double p2[3];

            originalPolygon->GetPoints()->GetPoint(i, p2);

            double pointsDistance = sqrt(pow(p2[0] - p1[0], 2.0) + pow(p2[1] - p1[1], 2.0));

            if (pointsDistance < distanceFilter) {
                i++;
            } else {
                double x = p1[0] + (p2[0] - p1[0]) * distanceFilter / pointsDistance;
                double y = p1[1] + (p2[1] - p1[1]) * distanceFilter / pointsDistance;

                p1[0] = x;
                p1[1] = y;
                p1[2] = 0.0;
                filteredPolygon->GetPoints()->InsertPoint(count, p1);
                filteredPolygon->GetPointIds()->InsertId(count, count);
                count++;
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

double MeshPolygon::area() {
    for (vtkIdType i = 0; i < filteredPolygon->GetPointIds()->GetNumberOfIds(); i++) {
        filteredPolygon->GetPointIds()->SetId(i, i);
    }

    return filteredPolygon->ComputeArea();
}

void MeshPolygon::setId(const uint &id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

uint MeshPolygon::getId() const {
    return id;
}

void MeshPolygon::setName(const QString &name) {
    this->name = name;
}

QString MeshPolygon::getName() const {
    return name;
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

QString MeshPolygon::getPolyDataAsString() {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkIdType count = 0;
    
    points->SetNumberOfPoints(originalPolygon->GetPoints()->GetNumberOfPoints() + filteredPolygon->GetPoints()->GetNumberOfPoints());
    
    for (vtkIdType i = 0; i < originalPolygon->GetPoints()->GetNumberOfPoints(); i++) {
        points->SetPoint(count++, originalPolygon->GetPoints()->GetPoint(i));
    }
    for (vtkIdType i = 0; i < filteredPolygon->GetPoints()->GetNumberOfPoints(); i++) {
        points->SetPoint(count++, filteredPolygon->GetPoints()->GetPoint(i));
    }
    
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
    
    polygons->InsertNextCell(originalPolygon);
    polygons->InsertNextCell(filteredPolygon);
    
    polyData->SetPoints(points);
    polyData->SetPolys(polygons);
    
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    
    writer->SetFileName("MeshPolygonData");
    writer->SetInputData(polyData);
    writer->WriteToOutputStringOn();
    writer->Update();
    writer->Write();
    
    return QString::fromStdString(writer->GetOutputString());
}

void MeshPolygon::loadPolygonsFromStringPolyData(const QString &polyDataStr) {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    
    reader->SetInputString(polyDataStr.toStdString());
    reader->ReadFromInputStringOn();
    reader->Update();
    
    vtkPolyData *polyData = reader->GetOutput();
    vtkPolygon *polygon = vtkPolygon::SafeDownCast(polyData->GetCell(0));
    originalPolygon = vtkSmartPointer<vtkPolygon>::New();
    
    originalPolygon->DeepCopy(polygon);
    
    polygon = vtkPolygon::SafeDownCast(polyData->GetCell(1));
    filteredPolygon = vtkSmartPointer<vtkPolygon>::New();
    
    filteredPolygon->DeepCopy(polygon);
}

void MeshPolygon::setMinimumAngle(const double &minimumAngle) {
    this->minimumAngle = minimumAngle;
}

double MeshPolygon::getMinimumAngle() const {
    return minimumAngle;
}

double MeshPolygon::getMinimumAngleInCGALRepresentation() const {
    // http://doc.cgal.org/latest/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html
    return pow(sin(minimumAngle * M_PI / 180), 2.0);
}

void MeshPolygon::setMaximumEdgeLength(const double &maximumEdgeLength) {
    this->maximumEdgeLength = maximumEdgeLength;
}

double MeshPolygon::getMaximumEdgeLength() const {
    return maximumEdgeLength;
}

void MeshPolygon::setInitialCriteria() {
    double *bounds = this->filteredPolygon->GetPoints()->GetBounds();
    double width = bounds[1] - bounds[0];
    double height = bounds[3] - bounds[2];
    double smallEdgeLength = std::min(width, height) / 10.0;
    double optimalEdgeLength = std::max(smallEdgeLength, DEFAULT_MAXIMUM_EDGE_LENGTH);

    this->maximumEdgeLength = optimalEdgeLength;
    this->minimumAngle = DEFAULT_MINIMUM_ANGLE;
}

bool MeshPolygon::isPersisted() const {
    return this->id != 0;
}