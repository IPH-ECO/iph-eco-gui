#include "include/domain/grid_data.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <GeographicLib/GeoCoords.hpp>
#include "include/exceptions/grid_data_exception.h"
#include "include/utility/cgal_definitions.h"

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

GridInformationType GridData::getGridInformationType() const {
    return gridInformationType;
}

void GridData::setGridInformationType(const GridInformationType &gridInformationType) {
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

QSet<GridDataPoint>& GridData::getGridDataPoints() {
    return this->dataPoints;
}

GridDataPolygon& GridData::getGridDataPolygon() {
    return this->dataPolygon;
}

double GridData::getMinValue() const {
    return minValue;
}

double GridData::getMaxValue() const {
    return maxValue;
}

void GridData::buildData() {
    QFile inputFileHandle(inputFile);

    if (!inputFileHandle.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw GridDataException(QString("Unable to open Input File. Error: %1").arg(inputFileHandle.errorString()));
    }

    QTextStream in(&inputFileHandle);
    QList<Point> dataPolygonTempList;
    bool isPolygonDataSet = false, isMinValueSet = false, isMaxValueSet = false;

    dataPoints.clear();
    dataPolygon.clear();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList coordinate = line.split(',');

        if (coordinate.count() == 3) {
            GeographicLib::GeoCoords utmCoordinate(coordinate.at(1).toDouble(), coordinate.at(0).toDouble());
            Point point(utmCoordinate.Easting(), utmCoordinate.Northing());
            double data = coordinate.at(2).toDouble();

            if (!isMinValueSet || data < minValue) {
                minValue = data;
                isMinValueSet = true;
            }
            if (!isMaxValueSet || data > maxValue) {
                maxValue = data;
                isMaxValueSet = true;
            }

            if (this->gridInputType == GridData::POINT) {
                GridDataPoint dataPoint(point, data);
                dataPoints.insert(dataPoint);
            } else {
                if (!isPolygonDataSet) {
                    dataPolygon.setData(coordinate.at(2).toDouble());
                    isPolygonDataSet = true;
                }
                // Avoid duplicates
                if (!dataPolygonTempList.contains(point)) {
                    dataPolygon.push_back(point);
                    dataPolygonTempList.append(point);
                }
            }
        }
    }

    inputFileHandle.close();

    if (dataPoints.isEmpty() && dataPolygon.is_empty()) {
        throw GridDataException("Error: invalid file contents.");
    }
}

void GridData::copy(GridData &srcGridData) {
    this->gridInputType = srcGridData.getGridInputType();
    this->gridInformationType = srcGridData.getGridInformationType();
    this->inputFile = srcGridData.getInputFile();
    this->show = srcGridData.getShow();
    this->minValue = srcGridData.getMinValue();
    this->maxValue = srcGridData.getMaxValue();

    if (srcGridData.getGridInputType() == POINT) {
        this->exponent = srcGridData.getExponent();
        this->radius = srcGridData.getRadius();
        this->dataPoints = srcGridData.getGridDataPoints();
    } else {
        this->dataPolygon = srcGridData.getGridDataPolygon();
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
