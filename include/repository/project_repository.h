#ifndef PROJECT_REPOSITORY_H
#define PROJECT_REPOSITORY_H

#include "include/domain/project.h"
#include "include/domain/grid_data_configuration.h"
#include <QSqlDatabase>

class ProjectRepository : public QObject {
    Q_OBJECT
private:
    QSqlDatabase database;
    QString databaseName;
    int currentProgress;
    bool operationCanceled;
    
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
    ProjectRepository(const QString &databaseName);

    void open();
    void save();
    
    int getMaximumSaveProgress();
    int getMaximumLoadProgress();

public slots:
    void cancelOperation();
    
signals:
    void updateProgress(int value);
    void updateProgressText(const QString &text);
};

#endif // PROJECT_REPOSITORY_H
