#ifndef GRID_DATA_H
#define GRID_DATA_H

#include <QObject>

class GridData : public QObject {
    Q_OBJECT
public:
    enum GridInputType { POINT = 1, POLYGON };
    enum GridInformationType { BATHYMETRY = 1, WIND_REDUCTION, CHEZY, WETLAND_AREA, UNDEFINED };

private:
    GridInputType gridInputType;
    QString inputFile;
    GridInformationType gridInformationType;
    double exponent;
    double radius;
    bool show;

public:
    GridData();

    QString getName() const;
    void setName(const QString &name);
    GridInputType getGridInputType() const;
    void setGridInputType(const GridInputType gridInputType);
    QString getInputFile() const;
    void setInputFile(const QString &inputFile);
    GridInformationType getGridInformationType() const;
    void setGridInformationType(const GridInformationType &gridInformationType);
    double getExponent() const;
    void setExponent(const double &exponent);
    double getRadius() const;
    void setRadius(const double &radius);

    bool getShow() const;

    QString gridInputTypeToString() const;
    QString gridInformationTypeToString() const;

    static GridInformationType toGridInformationType(const QString gridInformationTypeStr);

public slots:
    void setShow(bool show);
};

#endif // GRID_DATA_H
