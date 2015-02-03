#ifndef PROJECT_H
#define PROJECT_H

#include <QString>

class Project {
    private:
        qlonglong id;        
        QString name;
        QString description;
        QString filename;
        QString boundaryData;
        qlonglong selectedGridDataId;
        bool hydrodynamic;
        bool waterQuality;
        bool sediment;
        qlonglong selectedMeteorologicalDataId;
        int version;

    public:
        Project(QString &_name, QString &_description, bool &_hydrodynamic, bool &_sediment, bool &_waterQuality);

        void setId(const qlonglong &id);
        qlonglong getId() const;

        void setName(const QString &name);
        QString getName() const;

        void setDescription(const QString &description);
        QString getDescription() const;

        void setFilename(const QString &filename);
        QString getFilename() const;

        void setBoundaryData(const QString &boundaryData);
        QString getBoundaryData() const;

        void setSelectedGridDataId(const int &selectedGridDataId);
        qlonglong getSelectedGridDataId() const;

        void setHydrodynamic(const bool &hydrodynamic);
        bool getHydrodynamic() const;

        void setWaterQuality(const bool &waterQuality);
        bool getWaterQuality() const;

        void setSediment(const bool &sediment);
        bool getSediment() const;

        void setSelectedMeteorologicalDataId(const qlonglong &selectedMeteorologicalDataId);
        qlonglong getSelectedMeteorologicalDataId() const;

        void setVersion(const int &version);
        int getVersion() const;
};

#endif // PROJECT_H
