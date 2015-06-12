#include "include/domain/grid_data.h"

#include "include/exceptions/grid_data_exception.h"
#include "include/utility/cgal_definitions.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <GeographicLib/GeoCoords.hpp>
#include <vtkSimplePointsReader.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

GridData::GridData() : id(0) {}

uint GridData::getId() const {
    return id;
}

void GridData::setId(const uint &id) {
    this->id = id;
}

QString GridData::getName() const {
    return name;
}

void GridData::setName(const QString &name) {
    this->name = name;
}

GridDataInputType GridData::getGridDataInputType() const {
    return gridDataInputType;
}

void GridData::setGridDataInputType(const GridDataInputType &gridDataInputType) {
    this->gridDataInputType = gridDataInputType;
}

QString GridData::getInputFile() const {
    return inputFile;
}

void GridData::setInputFile(const QString &inputFile) {
    this->inputFile = inputFile;
}

GridDataType GridData::getGridDataType() const {
    return gridDataType;
}

void GridData::setGridDataType(const GridDataType &gridDataType) {
    this->gridDataType = gridDataType;
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

vtkPolyData* GridData::getData() const {
    return this->data;
}

void GridData::build() {
    vtkSmartPointer<vtkSimplePointsReader> reader = vtkSmartPointer<vtkSimplePointsReader>::New();

    reader->SetFileName(this->inputFile.toStdString().c_str());
    reader->Update();

    data = vtkSmartPointer<vtkPolyData>::New();
    data->DeepCopy(reader->GetOutput());
    
    vtkPoints *points = data->GetPoints();
    vtkIdType numberOfPoints = points->GetNumberOfPoints();

    if (numberOfPoints == 0) {
        throw GridDataException("No points returned in grid data file.");
    }
    
    vtkSmartPointer<vtkDoubleArray> scalars = vtkSmartPointer<vtkDoubleArray>::New();

    scalars->SetNumberOfComponents(1);
    
    for (vtkIdType i = 0; i < numberOfPoints; i++) {
        double point[3];

        points->GetPoint(i, point);

        GeographicLib::GeoCoords utmCoordinate(point[1], point[0]);
        points->SetPoint(i, utmCoordinate.Easting(), utmCoordinate.Northing(), 0.0);
        scalars->InsertNextTuple1(point[2]);
    }
    
    data->GetPointData()->SetScalars(scalars);
}

QString GridData::gridDataInputTypeToString() const {
    switch (gridDataInputType) {
    case GridDataInputType::POINT:
        return "Point";
    case GridDataInputType::POLYGON:
        return "Polygon";
    }
}

QString GridData::gridDataTypeToString() const {
    switch (gridDataType) {
        case GridDataType::BATHYMETRY:
            return "Bathymetry";
        case GridDataType::WIND_REDUCTION:
            return "Wind Reduction Coefficient";
        case GridDataType::CHEZY:
            return "Chezy Coefficient";
        case GridDataType::WETLAND_AREA:
            return "Wetland Area";
    }
}

GridDataType GridData::toGridDataType(const QString &gridDataTypeStr) {
    if (gridDataTypeStr == "Bathymetry") {
        return GridDataType::BATHYMETRY;
    }
    if (gridDataTypeStr == "Wind Reduction Coefficient") {
        return GridDataType::WIND_REDUCTION;
    }
    if (gridDataTypeStr == "Chezy Coefficient") {
        return GridDataType::CHEZY;
    }
    
    return GridDataType::WETLAND_AREA;
}