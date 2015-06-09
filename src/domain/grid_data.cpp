#include "include/domain/grid_data.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <GeographicLib/GeoCoords.hpp>
#include "include/exceptions/grid_data_exception.h"
#include "include/utility/cgal_definitions.h"

#include <vtkParticleReader.h>
#include <vtkDoubleArray.h>

GridData::GridData() {}

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

bool GridData::getShow() const {
    return show;
}

void GridData::setShow(bool show) {
    this->show = show;
}

vtkPolyData* GridData::getData() const {
    return this->data;
}

void GridData::build() {
    vtkSmartPointer<vtkParticleReader> reader = vtkSmartPointer<vtkParticleReader>::New();

    reader->SetFileName(this->inputFile.toStdString().c_str());
    reader->SetDataTypeToDouble();
    reader->SetFileTypeToText();
    reader->SetDataByteOrderToBigEndian();
    reader->Update();

    // if (!inputFileHandle.open(QIODevice::ReadOnly | QIODevice::Text)) {
    //     throw GridDataException(QString("Unable to open Input File. Error: %1").arg(inputFileHandle.errorString()));
    // }

    data = vtkSmartPointer<vtkPolyData>::New();
    data->DeepCopy(reader->GetOutput());
    
    vtkSmartPointer<vtkPoints> points = data->GetPoints();
    vtkIdType numberOfPoints = points->GetNumberOfPoints();

    if (numberOfPoints == 0) {
        throw GridDataException("No points returned in grid data file.");
    }

    for (vtkIdType i = 0; i < numberOfPoints; i++) {
        double point[3];

        points->GetPoint(i, point);

        GeographicLib::GeoCoords utmCoordinate(point[1], point[0]);
        points->SetPoint(i, utmCoordinate.Easting(), utmCoordinate.Northing(), 0.0);
    }
}

// void GridData::copy(GridData &srcGridData) {
//     this->gridDataInputType = srcGridData.getGridDataInputType();
//     this->gridDataType = srcGridData.getGridDataType();
//     this->inputFile = srcGridData.getInputFile();
//     this->show = srcGridData.getShow();

//     if (srcGridData.getGridDataInputType() == GridDataInputType::POINT) {
//         this->exponent = srcGridData.getExponent();
//         this->radius = srcGridData.getRadius();
//         this->dataPoints = srcGridData.getGridDataPoints();
//     } else {
//         this->dataPolygon = srcGridData.getGridDataPolygon();
//     }
// }

QString GridData::gridDataInputTypeToString() const {
    switch (gridDataInputType) {
    case GridDataInputType::POINT:
        return "Point";
    case GridDataInputType::POLYGON:
        return "Polygon";
    default:
        return "";
    }
}

