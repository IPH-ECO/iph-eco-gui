#ifndef PROJECT_H
#define PROJECT_H

#include <QSet>

#include "include/domain/mesh.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

class Project {
    private:
        qint8 id;
        QString name;
        QString description;
        QString filename;
        bool hydrodynamic;
        bool waterQuality;
        bool sediment;
        QSet<StructuredMesh> structuredMeshes;
        QSet<UnstructuredMesh> unstructuredMeshes;

    public:
        Project(QString &_name, QString &_description, bool &_hydrodynamic, bool &_sediment, bool &_waterQuality);

        void setId(const qint8 &id);
        qint8 getId() const;

        void setName(const QString &name);
        QString getName() const;

        void setDescription(const QString &description);
        QString getDescription() const;

        void setFilename(const QString &filename);
        QString getFilename() const;

        void setHydrodynamic(const bool &hydrodynamic);
        bool getHydrodynamic() const;

        void setWaterQuality(const bool &waterQuality);
        bool getWaterQuality() const;

        void setSediment(const bool &sediment);
        bool getSediment() const;

};

#endif // PROJECT_H
