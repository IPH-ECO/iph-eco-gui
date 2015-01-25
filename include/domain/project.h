#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

class Project {
    private:
//        long id;
        QString name;
        int version;
        QString description;
        QString analysis;
        long selectedGridDataId;
        long selectedHydrodynamicDataId;
        long selectedWaterQualityDataId;
        long selectedSedimentDataId;
        long selectedMeteorologicalDataId;

    public:
        Project(QString &name, QString &description, QString &analysis);

        void setName(const QString &name);
        const QString getName() const;

        void setVersion(const int &version);
        int getVersion() const;

        void setDescription(const QString &description);
        QString getDescription() const;

        void setAnalysis(const QString &analysis);
        QString getAnalysis() const;

        void setSelectedGridDataId(const int &selectedGridDataId);
        long getSelectedGridDataId() const;

        void setSelectedHydrodynamicDataId(const long &selectedHydrodynamicDataId);
        long getSelectedHydrodynamicDataId() const;

        void setSelectedWaterQualityDataId(const long &selectedWaterQualityDataId);
        long getSelectedWaterQualityDataId() const;

        void setSelectedSedimentDataId(const long &selectedSedimentDataId);
        long getSelectedSedimentDataId() const;

        void setSelectedMeteorologicalDataId(const long &selectedMeteorologicalDataId);
        long getSelectedMeteorologicalDataId() const;
};

#endif // PROJECT_H
