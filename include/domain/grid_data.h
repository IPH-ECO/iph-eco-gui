#ifndef GRID_DATA_H
#define GRID_DATA_H

#include <QObject>

#include "mesh.h"

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
    Mesh *mesh;

    //Transient
    bool show;

public:
    GridData();
    GridData(Mesh *mesh);

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
    Mesh* getMesh() const;
    void setMesh(Mesh *mesh);

    bool getShow() const;

    QString gridInputTypeToString() const;
    QString gridInformationTypeToString() const;

    static GridInformationType toGridInformationType(const QString gridInformationTypeStr);

public slots:
    void setShow(bool show);
};

#endif // GRID_DATA_H
