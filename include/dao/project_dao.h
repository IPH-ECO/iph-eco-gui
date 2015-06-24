#ifndef PROJECT_DAO_H
#define PROJECT_DAO_H

#include "include/domain/project.h"
#include "include/domain/grid_data_configuration.h"
#include <QSqlDatabase>

class ProjectDAO : public QObject {
    Q_OBJECT
private:
    QSqlDatabase database;
    QString databaseName;
    int currentProgress;
    bool saveCanceled;
    bool loadCanceled;
    
    void connectDB();
    
    void saveMeshes(Project *project);
    void saveMeshPolygons(Mesh *mesh);
    void loadMeshes(Project *project);
    void loadMeshPolygons(Mesh *mesh);
    void loadGridDataConfigurations(Project *project);
    void loadGridData(GridDataConfiguration *gridDataConfiguration, Project *project);
    
    void saveGridDataConfigurations(Project *project);
    void saveGridData(GridDataConfiguration *gridDataConfiguration);

public:
    ProjectDAO(const QString &_databaseName);

    Project* open();
    void save(Project *project);
    
    int getMaximumSaveProgress(Project *project);
    int getMaximumLoadProgress();

public slots:
    void cancelSave(bool value = true);
    
signals:
    void updateProgress(int value);
    void updateProgressText(const QString &text);
};

#endif // PROJECT_DAO_H
