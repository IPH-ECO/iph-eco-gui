#include <domain/grid_data.h>

#include <exceptions/grid_data_exception.h>

#include <GeographicLib/GeoCoords.hpp>
#include <vtkSimplePointsReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkPointLocator.h>
#include <vtkDoubleArray.h>
#include <vtkCellCenters.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <QApplication>
#include <vtkMath.h>

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

    vtkSmartPointer<vtkPolyData> meshPolyData = mesh->getMeshPolyData();
    vtkSmartPointer<vtkDoubleArray> interpolatedWeightsArray = vtkSmartPointer<vtkDoubleArray>::New();
    std::string gridDataName(this->name.toStdString());

	interpolatedWeightsArray->SetName(gridDataName.c_str());
    interpolatedWeightsArray->SetNumberOfComponents(1);
    interpolatedWeightsArray->SetNumberOfTuples(meshPolyData->GetNumberOfCells());
    
    vtkSmartPointer<vtkCellCenters> cellCentersFilter = vtkSmartPointer<vtkCellCenters>::New();
    cellCentersFilter->SetInputData(meshPolyData);
    cellCentersFilter->Update();
    
    vtkSmartPointer<vtkPolyData> cellCenters = cellCentersFilter->GetOutput();
    
    for (vtkIdType i = 0; i < meshPolyData->GetNumberOfCells() && !interpolationCanceled; i++) {
        double *cellCenter = cellCenters->GetPoint(i);
        double weight = 0.0;
        
        if (gridDataInputType == GridDataInputType::POINT) {
            vtkSmartPointer<vtkPointLocator> pointLocator = vtkSmartPointer<vtkPointLocator>::New();
            pointLocator->SetDataSet(inputPolyData);
            pointLocator->AutomaticOn();
            pointLocator->SetNumberOfPointsPerBucket(2);
            pointLocator->BuildLocator();
            
            vtkSmartPointer<vtkIdList> inscribedPointsIds = vtkSmartPointer<vtkIdList>::New();
            pointLocator->FindPointsWithinRadius(radius, cellCenter, inscribedPointsIds);

            if (inscribedPointsIds->GetNumberOfIds() == 1) {
                vtkIdType nearestPointId = pointLocator->FindClosestPoint(cellCenter);
                weight = inputPolyData->GetPointData()->GetScalars()->GetTuple1(nearestPointId);
            } else {
                weight = inverseOfDistance(inscribedPointsIds, cellCenter);
            }
        } else { // GridDataInputType::POLYGON
            double *inputPolyDataPointer = static_cast<double*>(inputPolyData->GetPoints()->GetData()->GetVoidPointer(0));
            double inputPointsBounds[6], normal[3] = { 0.0, 0.0, 1.0 };
            
            inputPolyData->GetPoints()->GetBounds(inputPointsBounds);
            
            if (vtkPolygon::PointInPolygon(cellCenter, inputPolyData->GetPoints()->GetNumberOfPoints(), inputPolyDataPointer, inputPointsBounds, normal)) {
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

double GridData::inverseOfDistance(vtkSmartPointer<vtkIdList> inscribedPointsIds, double *cellCenter) {
    double numerator = 0.0, denominator = 0.0;
    
    for (vtkIdType i = 0; i < inscribedPointsIds->GetNumberOfIds(); i++) {
        vtkIdType pointId = inscribedPointsIds->GetId(i);
        double distance = vtkMath::Distance2BetweenPoints(cellCenter, inputPolyData->GetPoints()->GetPoint(pointId));
        
        if (distance == 0) {
            continue;
        }
        
        double distancePow = pow(distance, exponent);
        numerator += inputPolyData->GetPointData()->GetScalars()->GetTuple1(pointId) / distancePow;
        denominator += 1.0 / distancePow;
    }
    
    return denominator == 0 ? 0 : numerator / denominator;
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
    
    if (!weights) {
        return 0;
    }
    
    double range[2];
    
    weights->GetRange(range);
    
    return range[0];
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