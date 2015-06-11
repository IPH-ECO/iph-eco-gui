#ifndef PROJECT_H
#define PROJECT_H

#include <QSet>

#include "mesh.h"
#include "grid_data_configuration.h"

class Project {
private:
    uint id;
    QString name;
    QString description;
    QString filename;
    bool hydrodynamic;
    bool waterQuality;
    bool sediment;
    QSet<Mesh*> meshes;
    QSet<GridDataConfiguration*> gridDataConfigurations;

    //Transient attributes
    bool dirty;

public:
    Project(const QString &name, const QString &description, const bool &hydrodynamic, const bool &sediment, const bool &waterQuality);

    void setId(const uint &id);
    uint getId() const;
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

    bool addMesh(Mesh *mesh);
    void removeMesh(Mesh *mesh);
    bool containsMesh(const QString &meshName);
    Mesh* getMesh(const QString &meshName) const;
    QSet<Mesh*> getMeshes() const;

    bool addGridDataConfiguration(GridDataConfiguration *gridDataConfiguration);
    void removeGridDataConfiguration(const QString &configurationName);
    GridDataConfiguration* getGridDataConfiguration(const QString &configurationName);
    QSet<GridDataConfiguration*> getGridDataConfigurations() const;

    //Transient gets and sets
    bool isPersisted() const;
    bool isDirty() const;
    void setDirty(const bool &dirty);
};

#endif // PROJECT_H
