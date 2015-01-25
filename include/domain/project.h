#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

class Project {
    private:
        qlonglong id;
        QString name;
        QString description;
        QString analysis;
        qlonglong selectedGridDataId;
        qlonglong selectedHydrodynamicDataId;
        qlonglong selectedWaterQualityDataId;
        qlonglong selectedSedimentDataId;
        qlonglong selectedMeteorologicalDataId;
        int version;

    public:
        Project(QString &_name, QString &_description, QString &_analysis);

        void setId(const qlonglong &id);
        qlonglong getId() const;

        void setName(const QString &name);
        QString getName() const;

        void setDescription(const QString &description);
        QString getDescription() const;

        void setAnalysis(const QString &analysis);
        QString getAnalysis() const;

        void setSelectedGridDataId(const int &selectedGridDataId);
        qlonglong getSelectedGridDataId() const;

        void setSelectedHydrodynamicDataId(const qlonglong &selectedHydrodynamicDataId);
        qlonglong getSelectedHydrodynamicDataId() const;

        void setSelectedWaterQualityDataId(const qlonglong &selectedWaterQualityDataId);
        qlonglong getSelectedWaterQualityDataId() const;

        void setSelectedSedimentDataId(const qlonglong &selectedSedimentDataId);
        qlonglong getSelectedSedimentDataId() const;

        void setSelectedMeteorologicalDataId(const qlonglong &selectedMeteorologicalDataId);
        qlonglong getSelectedMeteorologicalDataId() const;

        void setVersion(const int &version);
        int getVersion() const;
};

#endif // PROJECT_H
