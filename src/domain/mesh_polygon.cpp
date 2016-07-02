#if defined(_WIN32) || defined(_WIN64)
    #define _USE_MATH_DEFINES
#endif

#include <cmath>

#include <domain/mesh_polygon.h>
#include <exceptions/mesh_polygon_exception.h>

#include <QFile>
#include <QMultiMap>
#include <QIODevice>
#include <QFileInfo>
#include <pugixml.hpp>
#include <QStringList>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSimplePointsReader.h>
#include <GeographicLib/GeoCoords.hpp>

struct OuterBoundaryIsTagTreeWalker : public pugi::xml_tree_walker {
    int outerBoundaryIsCount;

    OuterBoundaryIsTagTreeWalker() : outerBoundaryIsCount(0) {}
    
    virtual bool for_each(pugi::xml_node &node) {
        if (strcmp(node.name(), "outerBoundaryIs") == 0) {
            outerBoundaryIsCount++;
        }
        
        return true;
    }
};

const QString MeshPolygon::BOUNDARY_POLYGON_NAME = "Boundary";

const double MeshPolygon::DEFAULT_MINIMUM_ANGLE = 20.7;

const double MeshPolygon::DEFAULT_MAXIMUM_EDGE_LENGTH = 0.5;

MeshPolygon::MeshPolygon() : id(0), latitudeAverage(0.0) {}

MeshPolygon::MeshPolygon(const QString &name, const QString &filename, const MeshPolygonType &meshPolygonType) :
    id(0), name(name), meshPolygonType(meshPolygonType), latitudeAverage(0.0), filename(filename) {}

void MeshPolygon::build(const CoordinateSystem &coordinateSystem) {
    if (this->filename.isEmpty()) {
        if (!originalPolygon) {
            throw MeshPolygonException("Unexpected behaviour: original mesh polygon not present.");
        }
        
        // Original polygon already built
        return;
    }
    
    QFileInfo fileInfo(this->filename);
    QString extension = fileInfo.suffix().toLower();
    
    if (extension == "kml") {
        readFromKMLFile(coordinateSystem);
    } else if (extension == "txt" || extension == "xyz") {
        readFromTextFile(coordinateSystem);
    }
}

bool kmlCoordinates(const pugi::xml_node &node) {
    return strcmp(node.name(), "coordinates") == 0 && strcmp(node.parent().parent().name(), "outerBoundaryIs") == 0;
}

void MeshPolygon::readFromKMLFile(const CoordinateSystem &coordinateSystem) {
    QByteArray filenameByteArray = this->filename.toLocal8Bit();
    const char *xmlFilename = filenameByteArray.constData();
    
    pugi::xml_document xmlDocument;
    pugi::xml_parse_result xmlParseResult = xmlDocument.load_file(xmlFilename);
    
    if (xmlParseResult.status == pugi::xml_parse_status::status_file_not_found) {
        throw MeshPolygonException("Boundary file not found.");
    }
    
    if (xmlParseResult.status != pugi::xml_parse_status::status_ok) {
        throw MeshPolygonException("Unable to parse KML file.");
    }
    
    OuterBoundaryIsTagTreeWalker treeWalker;
    
    xmlDocument.traverse(treeWalker);
    
    if (treeWalker.outerBoundaryIsCount == 0) {
        throw MeshPolygonException("No outerBoundaryIs tags founds in KML file.");
    }
    
    if (treeWalker.outerBoundaryIsCount > 1) {
        throw MeshPolygonException("Only one outerBoundaryIs tag is allowed in KML file.");
    }
    
    pugi::xml_node coordinatesNode = xmlDocument.find_node(kmlCoordinates);
    
    if (coordinatesNode.empty()) {
        throw MeshPolygonException("No coordinates information found in KML file.");
    }
    
    QStringList coordinates = QString(coordinatesNode.text().as_string()).trimmed().split(" ");
    const int coordinatesCount = coordinates.size() - 1; // KML repeats the first coordinate at the end of the list
    
    originalPolygon = vtkSmartPointer<vtkPolygon>::New();
    originalPolygon->GetPoints()->SetNumberOfPoints(coordinatesCount);
    originalPolygon->GetPointIds()->SetNumberOfIds(coordinatesCount);
    
    this->latitudeAverage = 0.0;
    
    for (int i = 0; i < coordinatesCount; i++) {
        QStringList coordinateStr = coordinates.at(i).split(",");
        double latitude = coordinateStr.at(1).toDouble();
        double point[3] = { 0.0 };
        
        if (coordinateSystem == CoordinateSystem::LATITUDE_LONGITUDE) {
            try {
                GeographicLib::GeoCoords utmCoordinate(latitude, coordinateStr.at(0).toDouble());
                point[0] = utmCoordinate.Easting();
                point[1] = utmCoordinate.Northing();
            } catch (const GeographicLib::GeographicErr&) {
                throw MeshPolygonException(QString("Latitude/longitude out of range at line %1").arg(i + 1).toStdString());
            }
        } else if (coordinateSystem == CoordinateSystem::UTM) { // No conversion needed
            point[0] = coordinateStr.at(0).toDouble();
            point[1] = latitude;
        } else {
            throw MeshPolygonException("Undefined coordinate system.");
        }
        
        originalPolygon->GetPoints()->SetPoint(i, point);
        originalPolygon->GetPointIds()->SetId(i, i);
        latitudeAverage += latitude;
    }
    
    latitudeAverage /= (double) coordinatesCount;
}

void MeshPolygon::readFromTextFile(const CoordinateSystem &coordinateSystem) {
    QByteArray filename = this->filename.toLocal8Bit();
    vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();
    reader->SetFileName(filename.constData());
    reader->Update();
    
    vtkIdType numberOfPoints = reader->GetOutput()->GetPoints()->GetNumberOfPoints();
    
    if (numberOfPoints == 0) {
        throw MeshPolygonException("No points returned in text file.");
    }
    
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
    originalPolygon = vtkSmartPointer<vtkPolygon>::New();
    originalPolygon->GetPoints()->SetNumberOfPoints(numberOfPoints);
    originalPolygon->GetPointIds()->SetNumberOfIds(numberOfPoints);
    
    for (vtkIdType i = 0; i < numberOfPoints; i++) {
        double point[3];
        
        polyData->GetPoints()->GetPoint(i, point);
        
        if (coordinateSystem == CoordinateSystem::LATITUDE_LONGITUDE) {
            try {
                GeographicLib::GeoCoords utmConverter(point[0], point[1]);
                
                point[0] = utmConverter.Easting();
                point[1] = utmConverter.Northing();
            } catch (const GeographicLib::GeographicErr&) {
                throw MeshPolygonException(QString("Latitude/longitude out of range at line %1.").arg(i + 1).toStdString());
            }
        } else if (coordinateSystem == CoordinateSystem::UTM) { // No conversion needed
        } else {
            throw MeshPolygonException("Undefined coordinate system.");
        }
        
        originalPolygon->GetPoints()->SetPoint(i, point);
        originalPolygon->GetPointIds()->SetId(i, i);
    }
}

void MeshPolygon::filter(const double &distanceFilter) {
    if (!originalPolygon || originalPolygon->GetPoints()->GetNumberOfPoints() == 0) {
        throw MeshPolygonException("Unable to filter polygon. Original mesh polygon not present.");
    }
    
    filteredPolygon = vtkSmartPointer<vtkPolygon>::New();

    if (distanceFilter == 0.0) {
        filteredPolygon->DeepCopy(originalPolygon);
    } else {
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

void MeshPolygon::setId(const uint &id) {
        this->id = id;
}

uint MeshPolygon::getId() const {
    return id;
}

bool MeshPolygon::isPersisted() const {
    return this->id != 0;
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

QString MeshPolygon::getOriginalPolyDataAsString() {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    
    points->SetNumberOfPoints(originalPolygon->GetPoints()->GetNumberOfPoints());
    
	for (vtkIdType i = 0; i < originalPolygon->GetPoints()->GetNumberOfPoints(); i++) {
		points->SetPoint(i, originalPolygon->GetPoints()->GetPoint(i));
		originalPolygon->GetPointIds()->SetId(i, i);
	}
    
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();
    
    polygons->InsertNextCell(originalPolygon);
    
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

QString MeshPolygon::getFilteredPolyDataAsString() {
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	points->SetNumberOfPoints(filteredPolygon->GetPoints()->GetNumberOfPoints());

	for (vtkIdType i = 0; i < filteredPolygon->GetPoints()->GetNumberOfPoints(); i++) {
		points->SetPoint(i, filteredPolygon->GetPoints()->GetPoint(i));
		filteredPolygon->GetPointIds()->SetId(i, i);
	}

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkCellArray> polygons = vtkSmartPointer<vtkCellArray>::New();

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

void MeshPolygon::loadOriginalPolygonFromStringPolyData(const QString &polyDataStr) {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    
    reader->SetInputString(polyDataStr.toStdString());
    reader->ReadFromInputStringOn();
    reader->Update();
    
    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
	originalPolygon = vtkSmartPointer<vtkPolygon>::New();
	originalPolygon->DeepCopy(polyData->GetCell(0));
}

void MeshPolygon::loadFilteredPolygonFromStringPolyData(const QString &polyDataStr) {
	vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();

	reader->SetInputString(polyDataStr.toStdString());
	reader->ReadFromInputStringOn();
	reader->Update();

	vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();
	filteredPolygon = vtkSmartPointer<vtkPolygon>::New();
	filteredPolygon->DeepCopy(polyData->GetCell(0));
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

double MeshPolygon::getLatitudeAverage() const {
    return latitudeAverage;
}

void MeshPolygon::setLatitudeAverage(double latitudeAverage) {
    this->latitudeAverage = latitudeAverage;
}