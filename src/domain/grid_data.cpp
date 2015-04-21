#include "include/domain/grid_data.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <GeographicLib/GeoCoords.hpp>
#include "include/exceptions/grid_data_exception.h"

GridData::GridData() {}

GridData::GridInputType GridData::getGridInputType() const {
    return gridInputType;
}

void GridData::setGridInputType(const GridData::GridInputType gridInputType) {
    this->gridInputType = gridInputType;
}

QString GridData::getInputFile() const {
    return inputFile;
}

void GridData::setInputFile(const QString &inputFile) {
    this->inputFile = inputFile;
}

GridData::GridInformationType GridData::getGridInformationType() const {
    return gridInformationType;
}

void GridData::setGridInformationType(const GridData::GridInformationType &gridInformationType) {
    this->gridInformationType = gridInformationType;
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

void GridData::buildDataPoints() {
    QFile inputFileHandle(inputFile);

    if (!inputFileHandle.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw GridDataException(QString("Unable to open XYZ file. Error: %1").arg(inputFileHandle.errorString()));
    }

    QTextStream in(&inputFileHandle);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList coordinate = line.split(',');

        if (coordinate.count() == 3) {
            GeographicLib::GeoCoords utmCoordinate(coordinate.at(1).toDouble(), coordinate.at(0).toDouble());
            GridDataPoint dataPoint(utmCoordinate.Easting(), utmCoordinate.Northing(), coordinate.at(2).toDouble());

            gridDataPoints.insert(dataPoint);
        }
    }

    inputFileHandle.close();

    if (gridDataPoints.isEmpty()) {
        throw GridDataException("Error: invalid XYZ file.");
    }
}

QString GridData::gridInputTypeToString() const {
    switch (gridInputType) {
    case POINT:
        return "Point";
    case POLYGON:
        return "Polygon";
    default:
        return "";
    }
}

QString GridData::gridInformationTypeToString() const {
    switch (gridInformationType) {
    case BATHYMETRY:
        return "Bathymetry";
    case WIND_REDUCTION:
        return "Wind Reduction Coefficient";
    case CHEZY:
        return "Chezy Coefficient";
    case WETLAND_AREA:
        return "Wetland Area";
    default:
        return "";
    }
}

GridData::GridInformationType GridData::toGridInformationType(const QString gridInformationTypeStr) {
    if (gridInformationTypeStr == "Bathymetry") {
        return BATHYMETRY;
    }
    if (gridInformationTypeStr == "Wind Reduction Coefficient") {
        return WIND_REDUCTION;
    }
    if (gridInformationTypeStr == "Chezy Coefficient") {
        return CHEZY;
    }
    if (gridInformationTypeStr == "Wetland Area") {
        return WETLAND_AREA;
    }

    return UNDEFINED;
}
