#ifndef PROJECT_REPOSITORY_H
#define PROJECT_REPOSITORY_H

#include <domain/project.h>
#include <domain/grid_data_configuration.h>
#include <utility/database_utility.h>

class ProjectRepository : public QObject {
    Q_OBJECT
private:
    DatabaseUtility *databaseUtility;
    QString databaseName;
    int currentProgress;
    bool operationCanceled;
    bool makeCopy;
    
    void updateProgressAndProcessEvents();
    
    void saveMeshes(Project *project);
    void saveMeshPolygons(Mesh *mesh);
    void loadMeshes(Project *project);
    void loadMeshPolygons(Mesh *mesh);
    void loadGridDataConfigurations(Project *project);
    void loadGridData(GridDataConfiguration *gridDataConfiguration, Project *project);
    
    void loadHydrodynamicConfigurations(Project *project);
    void loadHydrodynamicParameter(HydrodynamicConfiguration *configuration, Project *project);
    void loadBoundaryConditions(HydrodynamicConfiguration *configuration);
    void loadVerticalIntegratedRanges(HydrodynamicBoundaryCondition *boundaryCondition);
    template<typename T> void loadTimeSeries(T *t);
    
    void loadWaterQualityConfigurations(Project *project);
    void loadWaterQualityParameters(WaterQualityConfiguration *configuration);
    void loadFoodMatrix(WaterQualityConfiguration *configuration);
    void loadBoundaryConditions(WaterQualityConfiguration *configuration);
    
    void saveGridDataConfigurations(Project *project);
    void saveGridData(GridDataConfiguration *gridDataConfiguration);

    void saveHydrodynamicConfigurations(Project *project);
    void saveHydrodynamicParameters(HydrodynamicConfiguration *configuration);
    void saveBoundaryConditions(HydrodynamicConfiguration *configuration);
    void saveVerticalIntegratedRanges(HydrodynamicBoundaryCondition *boundaryCondition);
    template<typename T> void saveTimeSeries(T *t);
    void saveTimeSeries(const uint &objectId, const QString &objectType, const QList<TimeSeries*> &timeSeriesList);
    
    void saveWaterQualityConfigurations(Project *project);
    void saveWaterQualityParameters(WaterQualityConfiguration *configuration);
    void saveFoodMatrix(WaterQualityConfiguration *configuration);
    void saveBoundaryConditions(WaterQualityConfiguration *configuration);
    
    void saveMeteorologicalConfigurations(Project *project);
    void saveMeteorologicalStations(MeteorologicalConfiguration *configuration);
    void saveMeteorologicalParameters(MeteorologicalStation *station);
    void loadMeteorologicalConfigurations(Project *project);
    void loadMeteorologicalStations(MeteorologicalConfiguration *configuration);
    void loadMeteorologicalParameters(MeteorologicalStation *station);
    
    void saveSimulations(Project *project);
    void loadSimulations(Project *project);

public:
    ProjectRepository(const QString &databaseName);

    void open();
    void close();
    void save(const bool &makeCopy = false);
    void saveSimulation(Simulation *simulation);
    
    int getMaximumSaveProgress();
    int getMaximumLoadProgress();

public slots:
    void cancelOperation();
    
signals:
    void updateProgress(int value);
    void updateProgressText(const QString &text);
};

#endif // PROJECT_REPOSITORY_H
