#include <domain/grid_data.h>

#include <domain/color_gradient.h>
#include <utility/cgal_definitions.h>
#include <exceptions/grid_data_exception.h>

#include <GeographicLib/GeoCoords.hpp>
#include <vtkRegularPolygonSource.h>
#include <vtkSimplePointsReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkDoubleArray.h>
#include <vtkIdTypeArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkTriangle.h>
#include <QApplication>
#include <vtkQuad.h>

GridData::GridData(Mesh *mesh) :
    id(0),
    layerProperties(new LayerProperties()),
    mesh(mesh),
    interpolationCanceled(false)
{}

GridData::~GridData() {
    delete layerProperties;
}

QMap<GridDataType, QString> GridData::gridTypesMap = {
    std::pair<GridDataType, QString>(GridDataType::BATHYMETRY, "Sediment level (bathymetry)*"),
    std::pair<GridDataType, QString>(GridDataType::ROUGHNESS, "Roughness*"),
    std::pair<GridDataType, QString>(GridDataType::WIND_REDUCTION, "Wind reduction coefficient"),
    std::pair<GridDataType, QString>(GridDataType::WETLAND_AREA, "Wetland areas"),
    std::pair<GridDataType, QString>(GridDataType::D50_GRAIN_SIZE, "D50 grain size"),
    std::pair<GridDataType, QString>(GridDataType::FRACTION_OF_ORGANIC_MATTER, "Fraction of organic matter in the sediment"),
    std::pair<GridDataType, QString>(GridDataType::IMPERVIOUS_BEDROCK_LEVEL, "Impervious bedrock level")
};

uint GridData::getId() const {
    return id;
}

void GridData::setId(const uint &id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

QString GridData::getName() const {
    return name;
}

void GridData::setName(const QString &name) {
    if (!this->name.isEmpty()) {
        std::string stdOldName = this->name.toStdString();
        
        if (mesh->getMeshPolyData()->GetCellData()->HasArray(stdOldName.c_str())) {
            std::string stdNewName = name.toStdString();
            mesh->getMeshPolyData()->GetCellData()->GetArray(stdOldName.c_str())->SetName(stdNewName.c_str());
        }
    }
    
    this->name = name;
}

GridDataInputType GridData::getGridDataInputType() const {
    return gridDataInputType;
}

void GridData::setGridDataInputType(const GridDataInputType &gridDataInputType) {
    this->gridDataInputType = gridDataInputType;
}

GridDataType GridData::getGridDataType() const {
    return gridDataType;
}

void GridData::setGridDataType(const GridDataType &gridDataType) {
    this->gridDataType = gridDataType;
}

vtkPolyData* GridData::getInputPolyData() {
    return inputPolyData;
}

double GridData::getExponent() const {
    return exponent;
}

void GridData::setExponent(const double &exponent) {
    this->exponent = exponent;
}

double GridData::getRadius() const {
    return radius;
}

void GridData::setRadius(const double &radius) {
    this->radius = radius;
}

GridDataConfiguration* GridData::getGridDataConfiguration() const {
    return gridDataConfiguration;
}

void GridData::setGridDataConfiguration(GridDataConfiguration *gridDataConfiguration) {
    this->gridDataConfiguration = gridDataConfiguration;
}

LayerProperties* GridData::getLayerProperties() const {
    return layerProperties;
}

void GridData::setLayerProperties(LayerProperties *layerProperties) {
    this->layerProperties = layerProperties;
}

Mesh* GridData::getMesh() const {
    return mesh;
}

void GridData::setMesh(Mesh *mesh) {
    this->mesh = mesh;
}

CoordinateSystem GridData::getCoordinateSystem() const {
    return coordinateSystem;
}

void GridData::setCoordinateSystem(const CoordinateSystem &coordinateSystem) {
    this->coordinateSystem = coordinateSystem;
}

QString GridData::getInputFile() const {
    return inputFile;
}

void GridData::setInputFile(const QString &inputFile) {
    this->inputFile = inputFile;
}

void GridData::buildInputPolyData() {
    emit updateProgressText("Reading input file...");
	emit updateProgress(0);
	QApplication::processEvents();
    
    vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();
    reader->SetFileName(this->inputFile.toStdString().c_str());
    reader->Update();

    vtkIdType numberOfPoints = reader->GetOutput()->GetPoints()->GetNumberOfPoints();
    
    if (numberOfPoints == 0) {
        throw GridDataException("No points returned in grid data file.");
    }
    
    inputPolyData = vtkSmartPointer<vtkPolyData>::New();
    inputPolyData->DeepCopy(reader->GetOutput());
    
    vtkPoints *inputPoints = inputPolyData->GetPoints();
    
    vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray>::New();
	weights->SetName("inputPoints");
    weights->SetNumberOfComponents(1);
    weights->SetNumberOfTuples(numberOfPoints);
    
    for (vtkIdType i = 0; i < numberOfPoints; i++) {
        double point[3];

        inputPoints->GetPoint(i, point);

        if (coordinateSystem == CoordinateSystem::LATITUDE_LONGITUDE) {
            try {
                GeographicLib::GeoCoords utmCoordinate(point[1], point[0]);
                inputPoints->SetPoint(i, utmCoordinate.Easting(), utmCoordinate.Northing(), 0.0);
            } catch (const GeographicLib::GeographicErr&) {
                throw GridDataException(QString("Latitude/longitude out of range at line %1.").arg(i + 1).toStdString());
            }
        }
        
        weights->SetTuple1(i, point[2]);
    }
    
    inputPolyData->GetPointData()->SetScalars(weights);
    
    emit updateProgress(1);
}

void GridData::interpolate() {
    buildInputPolyData();
    
	emit updateProgressText("Interpolating grid data...");
    QApplication::processEvents();

    vtkPolyData *meshPolyData = mesh->getMeshPolyData();
    vtkSmartPointer<vtkDoubleArray> interpolatedWeightsArray = vtkSmartPointer<vtkDoubleArray>::New();
    std::string gridDataName(this->name.toStdString());

	interpolatedWeightsArray->SetName(gridDataName.c_str());
    interpolatedWeightsArray->SetNumberOfComponents(1);
    interpolatedWeightsArray->SetNumberOfTuples(meshPolyData->GetNumberOfCells());
    
    double cellCenter[3], inputPointsBounds[6], normal[3] = { 0.0, 0.0, 1.0 };
    vtkPoints *inputPoints = inputPolyData->GetPoints();
    
    inputPoints->GetBounds(inputPointsBounds);
    
    for (vtkIdType i = 0; i < meshPolyData->GetNumberOfCells() && !interpolationCanceled; i++) {
        double weight = 0.0;
        
        if (mesh->instanceOf("UnstructuredMesh")) {
            vtkTriangle *triangle = vtkTriangle::SafeDownCast(meshPolyData->GetCell(i));
            double *a = triangle->GetPoints()->GetPoint(0);
            double *b = triangle->GetPoints()->GetPoint(1);
            double *c = triangle->GetPoints()->GetPoint(2);
            
            vtkTriangle::TriangleCenter(a, b, c, cellCenter);
        } else {
            vtkQuad *quad = vtkQuad::SafeDownCast(meshPolyData->GetCell(i));
            double quadBounds[6];
            
            quad->GetBounds(quadBounds);
            cellCenter[0] = quadBounds[0] + (quadBounds[1] - quadBounds[0]) / 2.0;
            cellCenter[1] = quadBounds[2] + (quadBounds[3] - quadBounds[2]) / 2.0;
            cellCenter[2] = 0.0;
        }
        
        if (gridDataInputType == GridDataInputType::POINT) {
            vtkSmartPointer<vtkRegularPolygonSource> circleSource = vtkSmartPointer<vtkRegularPolygonSource>::New();
            circleSource->SetNumberOfSides(50);
            circleSource->SetRadius(radius);
            circleSource->SetCenter(cellCenter);
            circleSource->Update();
            
            vtkPolyData *circlePolyData = circleSource->GetOutput();
            vtkIdType circleNumberOfPoints = circlePolyData->GetPoints()->GetNumberOfPoints();
            double *circleDataPointer = static_cast<double*>(circlePolyData->GetPoints()->GetData()->GetVoidPointer(0));
            double circleBounds[6], point[3];
            
            circlePolyData->GetPoints()->GetBounds(circleBounds);
            
            vtkSmartPointer<vtkIdTypeArray> inscribedPointsIndexes = vtkSmartPointer<vtkIdTypeArray>::New();
            inscribedPointsIndexes->SetNumberOfComponents(1);
            
            for (vtkIdType j = 0; j < inputPoints->GetNumberOfPoints() && !interpolationCanceled; j++) {
                inputPoints->GetPoint(j, point);
                
                if (vtkPolygon::PointInPolygon(point, circleNumberOfPoints, circleDataPointer, circleBounds, normal)) {
                    inscribedPointsIndexes->InsertNextTuple1(j);
                }
            }
            
            if (interpolationCanceled) {
                break;
            }
            
            if (inscribedPointsIndexes->GetNumberOfTuples() == 0) {
                weight = calculateNearestWeight(cellCenter);
            } else {
                weight = inverseOfDistance(inscribedPointsIndexes, cellCenter);
            }
        } else { // GridDataInputType::POLYGON
            double *gridDataPointer = static_cast<double*>(inputPoints->GetData()->GetVoidPointer(0));
            
            if (vtkPolygon::PointInPolygon(cellCenter, inputPoints->GetNumberOfPoints(), gridDataPointer, inputPointsBounds, normal)) {
                weight = inputPolyData->GetPointData()->GetScalars()->GetTuple1(0);
            }
        }
        
        interpolatedWeightsArray->SetTuple1(i, weight);
        emit updateProgress(i + 1);
        QApplication::processEvents();
    }
    
    if (!interpolationCanceled) {
        meshPolyData->GetCellData()->RemoveArray(gridDataName.c_str());
        meshPolyData->GetCellData()->AddArray(interpolatedWeightsArray);
    }
}

double GridData::inverseOfDistance(vtkIdTypeArray *inscribedPointsIndexes, double *cellCenter) {
    double numerator = 0.0, denominator = 0.0;
    
    for (vtkIdType i = 0; i < inscribedPointsIndexes->GetNumberOfTuples(); i++) {
        vtkIdType tupleIndex = inscribedPointsIndexes->GetTuple1(i);
        double point[3];
        
        inputPolyData->GetPoints()->GetPoint(tupleIndex, point);
        
        double distance = sqrt(pow(point[0] - cellCenter[0], 2.0) + pow(point[1] - cellCenter[1], 2.0));
        double distancePow = pow(distance, exponent);
        numerator += inputPolyData->GetPointData()->GetScalars()->GetTuple1(tupleIndex) / distancePow;
        denominator += 1.0 / distancePow;
    }
    
    return numerator / denominator;
}

double GridData::calculateNearestWeight(double *cellCenter) {
    vtkIdType nearestId = 0;
    double point[3];
    
    inputPolyData->GetPoints()->GetPoint(nearestId, point);
    
    double minimalDistance = sqrt(pow(point[0] - cellCenter[0], 2.0) + pow(point[1] - cellCenter[1], 2.0));
    
    for (vtkIdType i = 1; i < inputPolyData->GetPoints()->GetNumberOfPoints(); i++) {
        inputPolyData->GetPoints()->GetPoint(i, point);
        
        double distance = sqrt(pow(point[0] - cellCenter[0], 2.0) + pow(point[1] - cellCenter[1], 2.0));
        
        if (minimalDistance > distance) {
            nearestId = i;
            minimalDistance = distance;
        }
    }
    
    return inputPolyData->GetPointData()->GetScalars()->GetTuple1(nearestId);
}

void GridData::cancelInterpolation(bool value) {
    this->interpolationCanceled = value;
}

QString GridData::gridDataInputTypeToString() const {
	if (gridDataInputType == GridDataInputType::POINT) {
		return "Point";
	}

	return "Polygon";
}

QMap<GridDataType, QString> GridData::getGridTypesMap() {
    return gridTypesMap;
}

void GridData::loadInputPolyDataFromStringPolyData(const QString &polyDataStr) {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    
    reader->SetInputString(polyDataStr.toStdString());
    reader->ReadFromInputStringOn();
    reader->Update();
    
    inputPolyData = vtkSmartPointer<vtkPolyData>::New();
    inputPolyData->DeepCopy(reader->GetOutput());
}

QString GridData::getInputPolyDataAsString() const {
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    
    writer->SetFileName("InputPolyData");
    writer->SetInputData(this->inputPolyData);
    writer->WriteToOutputStringOn();
    writer->Update();
    writer->Write();
    
    return QString::fromStdString(writer->GetOutputString());
}

bool GridData::isPersisted() const {
    return id != 0;
}

int GridData::getMaximumProgress() const {
	return mesh->getMeshPolyData()->GetNumberOfCells();
}

double GridData::getMinimumWeight() const {
    std::string arrayName = this->name.toStdString();
    vtkSmartPointer<vtkDoubleArray> weights = vtkDoubleArray::SafeDownCast(this->mesh->getMeshPolyData()->GetCellData()->GetArray(arrayName.c_str()));
    double minimum = weights->GetTuple1(0);
    
    for (int i = 1; i < weights->GetNumberOfTuples(); i++) {
        double weight = weights->GetTuple1(i);
        
        if (weight < minimum) {
            minimum = weight;
        }
    }
    
    return minimum;
}

SimulationDataType::GridData GridData::toSimulationDataType() const {
    SimulationDataType::GridData gridData;
    std::string arrayName = this->name.toStdString();
    vtkSmartPointer<vtkDoubleArray> weights = vtkDoubleArray::SafeDownCast(this->mesh->getMeshPolyData()->GetCellData()->GetArray(arrayName.c_str()));
    vtkIdType numberOfTuples = weights->GetNumberOfTuples();
 
    gridData.numberOfElements = numberOfTuples;
    gridData.weights = new double[numberOfTuples];
    gridData.typeId = (int) this->gridDataType;
    
    for (vtkIdType i = 0; i < numberOfTuples; i++) {
        gridData.weights[i] = weights->GetTuple1(i);
    }
    
    return gridData;
}