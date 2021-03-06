#include <repository/project_repository.h>

#include <application/iph_application.h>
#include <exceptions/database_exception.h>
#include <domain/unstructured_mesh.h>
#include <domain/structured_mesh.h>
#include <repository/simulation_repository.h>

#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>

ProjectRepository::ProjectRepository(const QString &databaseName) :
    databaseUtility(DatabaseUtility::getInstance()),
    databaseName(databaseName),
    currentProgress(0),
    operationCanceled(false)
{}

void ProjectRepository::updateProgressAndProcessEvents() {
    emit updateProgress(currentProgress++);
    QApplication::processEvents();
}

void ProjectRepository::open() {
    currentProgress = 0;
    emit updateProgressText("Loading project...");
    updateProgressAndProcessEvents();
    
    databaseUtility->connect(this->databaseName, true);
    
    if (!databaseUtility->isDatabaseValid()) {
        throw DatabaseException("Invalid IPH-ECO Project file.");
    }
    
    QSqlQuery query(databaseUtility->getDatabase());

    query.prepare("select * from project limit 1");
    query.exec();
    query.next();

    QString name = query.value("name").toString();
    QString description = query.value("description").toString();
    bool hydrodynamic = query.value("hydrodynamic").toBool();
    bool waterQuality = query.value("water_quality").toBool();

    Project *project = new Project(name, description, hydrodynamic, waterQuality);
    project->setId(query.value("id").toUInt());
    project->setFilename(this->databaseName);
    IPHApplication::setCurrentProject(project);
    
    loadMeshes(project);
    loadGridDataConfigurations(project);
    loadHydrodynamicConfigurations(project);
    loadMeteorologicalConfigurations(project);
    loadSimulations(project);
}

void ProjectRepository::close() {
    databaseUtility->disconnect();
}

void ProjectRepository::loadMeshes(Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    emit updateProgressText("Loading meshes...");
    QApplication::processEvents();
    
    query.prepare("select * from mesh");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        QString meshType = query.value("type").toString();
        Mesh *mesh = NULL;
        
        if (meshType == "StructuredMesh") {
            mesh = new StructuredMesh();
            static_cast<StructuredMesh*>(mesh)->setResolution(query.value("resolution").toUInt());
        } else {
            mesh = new UnstructuredMesh();
        }
        
        mesh->setId(query.value("id").toUInt());
        mesh->setName(query.value("name").toString());
        mesh->setCoordinatesDistance(query.value("coordinates_distance").toDouble());
        mesh->loadMeshPolyDataFromString(query.value("mesh_poly_data").toString());
        mesh->loadBoundaryPolyDataFromString(query.value("boundary_poly_data").toString());
        project->addMesh(mesh);
        
        updateProgressAndProcessEvents();
        
        loadMeshPolygons(mesh);
    }
}

void ProjectRepository::loadMeshPolygons(Mesh *mesh) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("select * from mesh_polygon where mesh_id = " + QString::number(mesh->getId()));
    query.exec();
    
    while (query.next() && !operationCanceled) {
        MeshPolygon *meshPolygon = new MeshPolygon();
        meshPolygon->setId(query.value("id").toUInt());
        meshPolygon->setName(query.value("name").toString());
        meshPolygon->setMeshPolygonType(static_cast<MeshPolygonType>(query.value("type").toInt()));
		meshPolygon->loadOriginalPolygonFromStringPolyData(query.value("original_poly_data").toString());
		meshPolygon->loadFilteredPolygonFromStringPolyData(query.value("filtered_poly_data").toString());
		meshPolygon->setMinimumAngle(query.value("minimum_angle").toDouble());
        meshPolygon->setMaximumEdgeLength(query.value("maximum_edge_length").toDouble());
        meshPolygon->setLatitudeAverage(query.value("latitude_average").toDouble());
        
        mesh->addMeshPolygon(meshPolygon);
        
        updateProgressAndProcessEvents();
    }
}

void ProjectRepository::loadGridDataConfigurations(Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    emit updateProgressText("Loading grid data...");
    QApplication::processEvents();
    
    query.prepare("select * from grid_data_configuration");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        GridDataConfiguration *configuration = new GridDataConfiguration();
        configuration->setId(query.value("id").toUInt());
        configuration->setName(query.value("name").toString());
        project->addGridDataConfiguration(configuration);
        
        updateProgressAndProcessEvents();
        
        loadGridData(configuration, project);
    }
}

void ProjectRepository::loadGridData(GridDataConfiguration *gridDataConfiguration, Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("select * from grid_data where grid_data_configuration_id = " + QString::number(gridDataConfiguration->getId()));
    query.exec();
    
    while (query.next() && !operationCanceled) {
        Mesh *mesh = project->getMesh(query.value("mesh_id").toUInt());
        GridData *gridData = new GridData(mesh);
        
        gridData->setId(query.value("id").toUInt());
        gridData->setName(query.value("name").toString());
        gridData->setGridDataType(static_cast<GridDataType>(query.value("grid_type").toInt()));
        gridData->setGridDataInputType(static_cast<GridDataInputType>(query.value("input_type").toInt()));
        gridData->loadInputPolyDataFromStringPolyData(query.value("input_poly_data").toString());
        gridData->setExponent(query.value("exponent").toDouble());
        gridData->setRadius(query.value("radius").toDouble());
        gridData->setGridDataConfiguration(gridDataConfiguration);
        
        gridDataConfiguration->addGridData(gridData);
        
        updateProgressAndProcessEvents();
    }
}

void ProjectRepository::loadHydrodynamicConfigurations(Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    emit updateProgressText("Loading hydrodynamic data...");
    QApplication::processEvents();
    
    query.prepare("select * from hydrodynamic_configuration");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        HydrodynamicConfiguration *configuration = new HydrodynamicConfiguration();
        GridDataConfiguration *gridDataConfiguration = project->getGridDataConfiguration(query.value("grid_data_configuration_id").toUInt());

        configuration->setId(query.value("id").toUInt());
        configuration->setName(query.value("name").toString());
        configuration->setGridDataConfiguration(gridDataConfiguration);
        project->addHydrodynamicConfiguration(configuration);
        
        updateProgressAndProcessEvents();
        
        loadHydrodynamicParameter(configuration, project);
        loadBoundaryConditions(configuration);
    }
}

void ProjectRepository::loadHydrodynamicParameter(HydrodynamicConfiguration *configuration, Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("select * from hydrodynamic_parameter where hydrodynamic_configuration_id = " + QString::number(configuration->getId()));
    query.exec();
    
    while (query.next() && !operationCanceled) {
        HydrodynamicParameter *parameter = new HydrodynamicParameter();
        parameter->setId(query.value("id").toUInt());
        parameter->setName(query.value("name").toString());
        parameter->setType(HydrodynamicParameter::mapTypeFromString(query.value("type").toString()));
        parameter->setValue(query.value("value").toDouble());
        parameter->setSelected(query.value("selected").toBool());
        parameter->setEnabled(query.value("enabled").toBool());

        configuration->addHydrodynamicParameter(parameter);

        updateProgressAndProcessEvents();
    }
}

void ProjectRepository::loadBoundaryConditions(HydrodynamicConfiguration *configuration) {
    QSqlQuery query(databaseUtility->getDatabase());
    QString configurationId = QString::number(configuration->getId());
    QString inputModule = QString::number((int) InputModule::HYDRODYNAMIC);
    
    query.prepare("select * from boundary_condition where configuration_id = " + configurationId + " and input_module = " + inputModule);
    query.exec();
    
    while (query.next() && !operationCanceled) {
        BoundaryCondition *boundaryCondition = new BoundaryCondition();
        boundaryCondition->setId(query.value("id").toUInt());
        boundaryCondition->setType((BoundaryConditionType) query.value("type").toInt());
        boundaryCondition->setObjectIds(query.value("object_ids").toString());
        boundaryCondition->setFunction((BoundaryConditionFunction) query.value("function").toInt());
        boundaryCondition->setConstantValue(query.value("constant_value").toDouble());
        boundaryCondition->setInputModule((InputModule) query.value("input_module").toInt());
        boundaryCondition->setCellColor(query.value("cell_color").toString());
        boundaryCondition->setVerticalIntegratedOutflow(query.value("vertical_integrated_outflow").toBool());
        boundaryCondition->setMinimumElevation(query.value("minimum_elevation").toDouble());
        boundaryCondition->setMaximumElevation(query.value("maximum_elevation").toDouble());
        
        configuration->addBoundaryCondition(boundaryCondition);
        
        updateProgressAndProcessEvents();

        loadTimeSeries(boundaryCondition);
    }
}

void ProjectRepository::loadTimeSeries(BoundaryCondition *boundaryCondition) {
    QSqlQuery query(databaseUtility->getDatabase());
    QList<TimeSeries*> timeSeriesList;
    
    query.prepare("select * from time_series where object_id = " + QString::number(boundaryCondition->getId()) + " and object_type = 'BoundaryCondition'");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        TimeSeries *timeSeries = new TimeSeries();
        timeSeries->setTimeStamp(query.value("time_stamp").toInt());
        timeSeries->setValue1(query.value("value1").toDouble());
        timeSeries->setValue2(query.value("value2").toDouble());
        
        boundaryCondition->addTimeSeries(timeSeries);
        
        updateProgressAndProcessEvents();
    }
}

void ProjectRepository::loadTimeSeries(MeteorologicalParameter *meteorologicalParameter) {
    QSqlQuery query(databaseUtility->getDatabase());
    QList<TimeSeries*> timeSeriesList;
    
    query.prepare("select * from time_series where object_id = " + QString::number(meteorologicalParameter->getId()) + " and object_type = 'MeteorologicalParameter'");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        TimeSeries *timeSeries = new TimeSeries();
        timeSeries->setTimeStamp(query.value("time_stamp").toInt());
        timeSeries->setValue1(query.value("value1").toDouble());
        timeSeries->setValue2(query.value("value2").toDouble());
        
        meteorologicalParameter->addTimeSeries(timeSeries);
        
        updateProgressAndProcessEvents();
    }
}

void ProjectRepository::loadMeteorologicalConfigurations(Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    emit updateProgressText("Loading meteorological data...");
    QApplication::processEvents();
    
    query.prepare("select * from meteorological_configuration");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        MeteorologicalConfiguration *configuration = new MeteorologicalConfiguration();
        configuration->setId(query.value("id").toUInt());
        configuration->setName(query.value("name").toString());
        configuration->setGridDataConfiguration(project->getGridDataConfiguration(query.value("grid_data_configuration_id").toUInt()));
        
        project->addMeteorologicalConfiguration(configuration);
        
        updateProgressAndProcessEvents();
        
        loadMeteorologicalStations(configuration);
    }
}

void ProjectRepository::loadMeteorologicalStations(MeteorologicalConfiguration *configuration) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("select * from meteorological_station");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        MeteorologicalStation *station = new MeteorologicalStation();
        station->setId(query.value("id").toUInt());
        station->setName(query.value("name").toString());
        station->setUseLatitudeLongitude(query.value("use_latitude_longitude").toBool());
        station->setUtmX(query.value("utm_x").toDouble());
        station->setUtmY(query.value("utm_y").toDouble());
        station->setLatitude(query.value("latitude").toDouble());
        station->setLongitude(query.value("longitude").toDouble());
        
        configuration->addStation(station);
        
        updateProgressAndProcessEvents();
        
        loadMeteorologicalParameters(station);
    }
}

void ProjectRepository::loadMeteorologicalParameters(MeteorologicalStation *station) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("select * from meteorological_parameter");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        MeteorologicalParameter *parameter = new MeteorologicalParameter();
        parameter->setId(query.value("id").toUInt());
        parameter->setName(query.value("name").toString());
        parameter->setUnit(query.value("unit").toString());
        parameter->setFunction((MeteorologicalParameterFunction) query.value("function").toInt());
        parameter->setConstantValue(query.value("constant_value").toDouble());
        parameter->setUseXYComponent(query.value("use_xy_component").toBool());
        parameter->setXComponent(query.value("x_component").toDouble());
        parameter->setYComponent(query.value("y_component").toDouble());
        parameter->setIntensity(query.value("intensity").toDouble());
        parameter->setDirection(query.value("direction").toDouble());
        
        station->addMeteorologicalParameter(parameter);
        
        updateProgressAndProcessEvents();
        
        loadTimeSeries(parameter);
    }
}

void ProjectRepository::loadSimulations(Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("select * from simulation order by label asc");
    query.exec();
    
    while (query.next() && !operationCanceled) {
        Simulation *simulation = new Simulation();
        simulation->setId(query.value("id").toUInt());
        simulation->setLabel(query.value("label").toString());
        simulation->setSimulationType((SimulationType) query.value("simulation_type").toInt());
        simulation->setStartTime(query.value("start_time").toUInt());
        simulation->setInitialTime(query.value("initial_time").toUInt());
        simulation->setPeriod(query.value("period").toDouble());
        simulation->setStepTime(query.value("step_time").toInt());
        simulation->setMinimumVerticalLimit(query.value("minimum_vertical_limit").toDouble());
        simulation->setMaximumVerticalLimit(query.value("maximum_vertical_limit").toDouble());
        simulation->setLayersFromString(query.value("layers").toString());
        simulation->setObservation(query.value("observations").toString());
        simulation->setOutputTimeInterval(query.value("output_time_interval").toInt());
        simulation->setOutputDirectory(query.value("output_directory").toString());
        simulation->setAutosaveTimeInterval(query.value("autosave_time_interval").toInt());
        simulation->setOutputParameters(query.value("output_parameters").toString().split(","));
        simulation->setStatus((SimulationStatus) query.value("status").toInt());
        simulation->setProgress(query.value("progress").toInt());
        simulation->setRecoveryVariables(query.value("recovery_variables").toString());
        
        HydrodynamicConfiguration *hydrodynamicConfiguration = project->getHydrodynamicConfiguration(query.value("hydrodynamic_configuration_id").toUInt());
        simulation->setHydrodynamicConfiguration(hydrodynamicConfiguration);
        
        MeteorologicalConfiguration *meteorologicalConfiguration = project->getMeteorologicalConfiguration(query.value("meteorological_configuration_id").toUInt());
        simulation->setMeteorologicalConfiguration(meteorologicalConfiguration);
        
        if (simulation->getStatus() == SimulationStatus::RUNNING) {
            SimulationRepository::updateSimulationStatus(simulation, SimulationStatus::PAUSED);
        }
        
        project->addSimulation(simulation);
        
        updateProgressAndProcessEvents();
    }
}

void ProjectRepository::save(const bool &makeCopy) {
    currentProgress = 0;
    emit updateProgressText("Saving project...");
    updateProgressAndProcessEvents();
    
    Project *project = IPHApplication::getCurrentProject();
    QString sql;
    
    this->makeCopy = makeCopy;
    databaseUtility->connect(this->databaseName, makeCopy);
    
    QSqlDatabase::database().transaction();
    try {
        if (project->isPersisted() && !makeCopy) {
            sql = "update project set name = :n, description = :d, hydrodynamic = :h, water_quality = :w";
        } else {
            databaseUtility->createApplicationTables();
            sql = "insert into project (name, description, hydrodynamic, water_quality) values (:n, :d, :h, :w)";
        }
        
        QSqlQuery query(databaseUtility->getDatabase());
        
        query.prepare(sql);
        query.bindValue(":n", project->getName());
        query.bindValue(":d", project->getDescription());
        query.bindValue(":h", project->getHydrodynamic());
        query.bindValue(":w", project->getWaterQuality());

        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save project. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        project->setId(1);
        saveMeshes(project);
        saveGridDataConfigurations(project);
        saveHydrodynamicConfigurations(project);
        saveMeteorologicalConfigurations(project);
        saveSimulations(project);
        
        if (operationCanceled) {
            QSqlDatabase::database().rollback();
            databaseUtility->revertConnection();
        } else {
            QSqlDatabase::database().commit();
            query.clear();
            query.exec("vacuum");
        }
    } catch (const DatabaseException &e) {
        QSqlDatabase::database().rollback();
        throw e;
    } catch (const std::exception &e) {
        throw DatabaseException(e.what());
    }
}

void ProjectRepository::saveMeshes(Project *project) {
    QSqlQuery query(databaseUtility->getDatabase());
    QSet<Mesh*> meshes = project->getMeshes();
    QStringList meshIds;
    
    if (!meshes.isEmpty()) {
        emit updateProgressText("Saving meshes...");
        QApplication::processEvents();
    }
    
    for (Mesh *mesh : meshes) {
        if (operationCanceled) {
            return;
        }
        
        QString sql;
        bool update = !this->makeCopy && mesh->isPersisted();
        
        if (update) {
            query.prepare("update mesh set name = :n, type = :t, mesh_poly_data = :mp, boundary_poly_data = :bp, coordinates_distance = :c, resolution = :r where id = :m");
            query.bindValue(":m", mesh->getId());
        } else {
            query.prepare("insert into mesh (name, type, coordinates_distance, mesh_poly_data, boundary_poly_data, resolution) values (:n, :t, :c, :mp, :bp, :r)");
        }
        
        QString meshType = mesh->instanceOf("StructuredMesh") ? "StructuredMesh" : "UnstructuredMesh";
        
        query.bindValue(":n", mesh->getName());
        query.bindValue(":t", meshType);
        query.bindValue(":c", mesh->getCoordinatesDistance());
        query.bindValue(":mp", mesh->getMeshPolyDataAsString());
        query.bindValue(":bp", mesh->getBoundaryPolyDataAsString());
        
        if (mesh->instanceOf("StructuredMesh")) {
            query.bindValue(":r", static_cast<StructuredMesh*>(mesh)->getResolution());
        } else {
            query.bindValue(":r", "NULL");
        }
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save meshes. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            mesh->setId(query.lastInsertId().toUInt());
        }
        
        meshIds.append(QString::number(mesh->getId()));
        saveMeshPolygons(mesh);
    }
    
    // Handle exclusions
    QString meshDeleteSql, meshPolygonDeleteSql;
    
    if (meshIds.isEmpty()) {
        meshDeleteSql = "delete from mesh";
        meshPolygonDeleteSql = "delete from mesh_polygon";
    } else {
        QString meshIdsStr = meshIds.join(",");
        
        meshDeleteSql = "delete from mesh where id not in (" + meshIdsStr + ")";
        meshPolygonDeleteSql = "delete from mesh_polygon where mesh_id not in (" + meshIdsStr + ")";
    }
    
    query.prepare(meshDeleteSql);
    query.exec();
    
    query.prepare(meshPolygonDeleteSql);
    query.exec();
}

void ProjectRepository::saveMeshPolygons(Mesh *mesh) {
    QList<MeshPolygon*> meshPolygons = mesh->getIslands() + mesh->getRefinementAreas();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList meshPolygonIds;
    
    meshPolygons.prepend(mesh->getBoundaryPolygon());
    
    for (MeshPolygon *meshPolygon : meshPolygons) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && meshPolygon->isPersisted();
        
        if (update) {
            QString sql = "update mesh_polygon set name = :n, type = :t, original_poly_data = :p, filtered_poly_data = :f, minimum_angle = :mi, maximum_edge_length = :ma %1 where id = :i";
            
            if (meshPolygon->getLatitudeAverage() != 0) {
                sql = sql.arg(", latitude_average = :l");
                query.bindValue(":l", meshPolygon->getLatitudeAverage());
            }
            
            query.prepare(sql);
            query.bindValue(":i", meshPolygon->getId());
        } else {
            query.prepare("insert into mesh_polygon (name, type, original_poly_data, filtered_poly_data, minimum_angle, maximum_edge_length, latitude_average, mesh_id) values (:n, :t, :p, :f, :mi, :ma, :l, :me)");
            query.bindValue(":me", mesh->getId());
        }
        
        query.bindValue(":n", meshPolygon->getName());
        query.bindValue(":t", (int) meshPolygon->getMeshPolygonType());
        query.bindValue(":p", meshPolygon->getOriginalPolyDataAsString());
		query.bindValue(":f", meshPolygon->getFilteredPolyDataAsString());
        if (mesh->instanceOf("UnstructuredMesh")) {
            query.bindValue(":mi", meshPolygon->getMinimumAngle());
            query.bindValue(":ma", meshPolygon->getMaximumEdgeLength());
        } else {
            query.bindValue(":mi", "NULL");
            query.bindValue(":ma", "NULL");
        }
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save project meshes. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            meshPolygon->setId(query.lastInsertId().toUInt());
        }
        
        meshPolygonIds.append(QString::number(meshPolygon->getId()));
    }
    
    query.prepare("delete from mesh_polygon where id not in (" + meshPolygonIds.join(",") + ") and mesh_id = " + QString::number(mesh->getId()));
    query.exec();
}

void ProjectRepository::saveGridDataConfigurations(Project *project) {
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList configurationIds;
    
    if (!configurations.isEmpty()) {
        emit updateProgressText("Saving grid data...");
        QApplication::processEvents();
    }
    
    for (GridDataConfiguration *configuration : configurations) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && configuration->isPersisted();

        if (update) {
            query.prepare("update grid_data_configuration set name = :n where id = :i");
            query.bindValue(":i", configuration->getId());
        } else {
            query.prepare("insert into grid_data_configuration (name) values (:n)");
        }
        
        query.bindValue(":n", configuration->getName());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save grid data configurations. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            configuration->setId(query.lastInsertId().toUInt());
        }
        
        configurationIds.append(QString::number(configuration->getId()));
        saveGridData(configuration);
    }
    
    // Handle exclusions
    QString configurationDeleteSql, gridDataDeleteSql;
    
    if (configurationIds.isEmpty()) {
        configurationDeleteSql = "delete from grid_data_configuration";
        gridDataDeleteSql = "delete from grid_data";
    } else {
        QString configurationIdsStr = configurationIds.join(",");
        
        configurationDeleteSql = "delete from grid_data_configuration where id not in (" + configurationIdsStr + ")";
        gridDataDeleteSql = "delete from grid_data where grid_data_configuration_id not in (" + configurationIdsStr + ")";
    }
    
    query.prepare(configurationDeleteSql);
    query.exec();
    
    query.prepare(gridDataDeleteSql);
    query.exec();
}

void ProjectRepository::saveGridData(GridDataConfiguration *gridDataConfiguration) {
    QVector<GridData*> gridDataVector = gridDataConfiguration->getGridDataVector();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList gridDataIds;
    
    for (GridData *gridData : gridDataVector) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && gridData->isPersisted();

        if (update) {
            query.prepare("update grid_data set name = :n, input_type = :it, grid_type = :gt, input_poly_data = :ipd, exponent = :e, radius = :r where id = :i");
            query.bindValue(":i", gridData->getId());
        } else {
            query.prepare("insert into grid_data (name, input_type, grid_type, input_poly_data, exponent, radius, grid_data_configuration_id, mesh_id) " \
                          "values (:n, :it, :gt, :ipd, :e, :r, :gdc, :m)");
            query.bindValue(":gdc", gridDataConfiguration->getId());
            query.bindValue(":m", gridData->getMesh()->getId());
        }
        
        query.bindValue(":n", gridData->getName());
        query.bindValue(":it", (int) gridData->getGridDataInputType());
        query.bindValue(":gt", (int) gridData->getGridDataType());
        query.bindValue(":ipd", gridData->getInputPolyDataAsString());
        
        if (gridData->getGridDataInputType() == GridDataInputType::POINT) {
            query.bindValue(":e", gridData->getExponent());
            query.bindValue(":r", gridData->getRadius());
        } else {
            query.bindValue(":e", "NULL");
            query.bindValue(":r", "NULL");
        }
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save grid data configurations. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            gridData->setId(query.lastInsertId().toUInt());
        }
        
        gridDataIds.append(QString::number(gridData->getId()));
    }
    
    query.prepare("delete from grid_data where id not in (" + gridDataIds.join(",") + ") and grid_data_configuration_id = " + QString::number(gridDataConfiguration->getId()));
    query.exec();
}

void ProjectRepository::saveHydrodynamicConfigurations(Project *project) {
    QSet<HydrodynamicConfiguration*> configurations = project->getHydrodynamicConfigurations();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList configurationIds;
    
    if (!configurations.isEmpty()) {
        emit updateProgressText("Saving hydrodynamic data...");
        QApplication::processEvents();
    }
    
    for (HydrodynamicConfiguration *configuration : configurations) {
        if (operationCanceled) {
            return;
        }
    
        bool update = !this->makeCopy && configuration->isPersisted();

        if (update) {
            query.prepare("update hydrodynamic_configuration set name = :n, grid_data_configuration_id = :g where id = :i");
            query.bindValue(":i", configuration->getId());
        } else {
            query.prepare("insert into hydrodynamic_configuration (name, grid_data_configuration_id) values (:n, :g)");
        }
        
        query.bindValue(":n", configuration->getName());
        query.bindValue(":g", configuration->getGridDataConfiguration()->getId());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save hydrodynamic data configurations. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            configuration->setId(query.lastInsertId().toUInt());
        }
        
        configurationIds.append(QString::number(configuration->getId()));
        saveHydrodynamicParameters(configuration);
        saveBoundaryConditions(configuration);
    }
    
    // Handle exclusions
    QStringList queries;

    if (configurationIds.isEmpty()) {
        queries << "delete from hydrodynamic_configuration";
        queries << "delete from hydrodynamic_parameter";
        queries << "delete from boundary_condition where input_module = " + QString::number((int) InputModule::HYDRODYNAMIC);
        // TODO: remove time series
    } else {
        QString configurationIdsStr = configurationIds.join(",");
        
        queries << "delete from hydrodynamic_configuration where id not in (" + configurationIdsStr + ")";
        queries << "delete from hydrodynamic_parameter where hydrodynamic_configuration_id not in (" + configurationIdsStr + ")";
        queries << "delete from boundary_condition where configuration_id not in (" + configurationIdsStr + ") and input_module = " + QString::number((int) InputModule::HYDRODYNAMIC);
    }
    
    for (QString sql : queries) {
        query.prepare(sql);
        query.exec();
    }
}

void ProjectRepository::saveHydrodynamicParameters(HydrodynamicConfiguration *configuration) {
    QList<HydrodynamicParameter*> parameters = configuration->getParameters();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList parameterIds;

    for (HydrodynamicParameter *parameter : configuration->getParameters()) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && parameter->isPersisted();
        
        if (update) {
            query.prepare("update hydrodynamic_parameter set value = :v, selected = :s where id = :i");
            query.bindValue(":i", parameter->getId());
        } else {
            query.prepare("insert into hydrodynamic_parameter (name, type, value, selected, enabled, hydrodynamic_configuration_id) values (:n, :t, :v, :s, :e, :h)");
            query.bindValue(":n", parameter->getName());
            query.bindValue(":t", HydrodynamicParameter::mapStringFromType(parameter->getType()));
            query.bindValue(":e", parameter->isEnabled());
            query.bindValue(":h", configuration->getId());
        }

        query.bindValue(":v", parameter->getValue());
        query.bindValue(":s", parameter->isSelected());

        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save hydrodynamic parameters. Error: %1.").arg(query.lastError().text()).toStdString());
        }

        updateProgressAndProcessEvents();

        QVariant lastInsertId = query.lastInsertId();
        
        if (!update) {
            parameter->setId(query.lastInsertId().toUInt());
        }
        
        parameterIds.append(QString::number(parameter->getId()));
    }
}

void ProjectRepository::saveBoundaryConditions(HydrodynamicConfiguration *configuration) {
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList boundaryConditionIds;

    for (BoundaryCondition *boundaryCondition : configuration->getBoundaryConditions()) {
        if (operationCanceled) {
            return;
        }

        bool update = !this->makeCopy && boundaryCondition->isPersisted();
        
        if (update) {
            query.prepare("update boundary_condition set type = :t, object_ids = :o, function = :f, constant_value = :c, cell_color = :cc, vertical_integrated_outflow = :v, minimum_elevation = :m1, maximum_elevation = :m2 where id = :i");
            query.bindValue(":i", boundaryCondition->getId());
        } else {
            query.prepare("insert into boundary_condition (type, object_ids, function, constant_value, input_module, cell_color, vertical_integrated_outflow, minimum_elevation, maximum_elevation, configuration_id) values (:t, :o, :f, :c, :i, :cc, :v, :m1, :m2, :ci)");
            query.bindValue(":i", (int) boundaryCondition->getInputModule());
            query.bindValue(":ci", configuration->getId());
        }

        query.bindValue(":t", (int) boundaryCondition->getType());
        query.bindValue(":o", boundaryCondition->getObjectIdsStr());
        query.bindValue(":f", (int) boundaryCondition->getFunction());
        query.bindValue(":c", boundaryCondition->getConstantValue());
        query.bindValue(":cc", boundaryCondition->getCellColor());
        query.bindValue(":v", boundaryCondition->useVerticalIntegratedOutflow());
        query.bindValue(":m1", boundaryCondition->getMinimumElevation());
        query.bindValue(":m2", boundaryCondition->getMaximumElevation());

        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save hydrodynamic boundary conditions. Error: %1.").arg(query.lastError().text()).toStdString());
        }

        updateProgressAndProcessEvents();

        if (!update) {
            boundaryCondition->setId(query.lastInsertId().toUInt());
        }
        
        boundaryConditionIds.append(QString::number(boundaryCondition->getId()));

        saveTimeSeries(boundaryCondition);
    }
    
    QString configurationIdStr = QString::number(configuration->getId());
    QString inputModuleStr = QString::number((int) InputModule::HYDRODYNAMIC);

    query.prepare("delete from boundary_condition where id not in (" + boundaryConditionIds.join(",") + ") and configuration_id = " + configurationIdStr + " and input_module = " + inputModuleStr);
    query.exec();
}

void ProjectRepository::saveTimeSeries(BoundaryCondition *boundaryCondition) {
    if (boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES && (makeCopy || boundaryCondition->isTimeSeriesChanged())) {
        saveTimeSeries(boundaryCondition->getId(), "BoundaryCondition", boundaryCondition->getTimeSeriesList());
		boundaryCondition->setTimeSeriesChanged(false);
    }
}

void ProjectRepository::saveTimeSeries(MeteorologicalParameter *parameter) {
    if (parameter->getFunction() == MeteorologicalParameterFunction::TIME_SERIES && (makeCopy || parameter->isTimeSeriesChanged())) {
        saveTimeSeries(parameter->getId(), "MeteorologicalParameter", parameter->getTimeSeriesList());
		parameter->setTimeSeriesChanged(false);
    }
}

void ProjectRepository::saveTimeSeries(const int &objectId, const QString &objectType, const QList<TimeSeries*> &timeSeriesList) {
    QSqlQuery query(databaseUtility->getDatabase());
    QString deleteSql = QString("delete from time_series where object_id = %2 and object_type = '%3'").arg(objectId).arg(objectType);

    query.exec(deleteSql);
    
    QStringList insertValues;
    int timeSeriesListSize = timeSeriesList.size();
    const int BATCH_LIMIT = 500;
    const int REMAINING = timeSeriesListSize % BATCH_LIMIT;
    int i = 1;
    
    for (TimeSeries *timeSeries : timeSeriesList) {
        if (operationCanceled) {
            return;
        }
        
        insertValues << QString("(%1,%2,%3,%4,'%5')").arg(timeSeries->getTimeStamp()).arg(timeSeries->getValue1()).arg(timeSeries->getValue2()).arg(objectId).arg(objectType);
        
        if (i % BATCH_LIMIT == 0 || (i == timeSeriesListSize && REMAINING > 0)) {
            QString insertQuery("insert into time_series (time_stamp, value1, value2, object_id, object_type) values ");
            
            insertQuery += insertValues.join(",");
            
            if (!query.exec(insertQuery)) {
                throw DatabaseException(QString("Unable to save time stamps. Error: %1.").arg(query.lastError().text()).toStdString());
            }
            
            insertValues.clear();
        }
        
        updateProgressAndProcessEvents();
        
        i++;
    }
}

void ProjectRepository::saveMeteorologicalConfigurations(Project *project) {
    QSet<MeteorologicalConfiguration*> configurations = project->getMeteorologicalConfigurations();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList configurationIds;
    
    if (!configurations.isEmpty()) {
        emit updateProgressText("Saving meterological data...");
        QApplication::processEvents();
    }
    
    for (MeteorologicalConfiguration *configuration : configurations) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && configuration->isPersisted();

        if (update) {
            query.prepare("update meteorological_configuration set name = :n, grid_data_configuration_id = :g where id = :i");
            query.bindValue(":i", configuration->getId());
        } else {
            query.prepare("insert into meteorological_configuration (name, grid_data_configuration_id) values (:n, :g)");
        }
        
        query.bindValue(":n", configuration->getName());
        query.bindValue(":g", configuration->getGridDataConfiguration()->getId());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save meteorological data configurations. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            configuration->setId(query.lastInsertId().toUInt());
        }
        
        configurationIds.append(QString::number(configuration->getId()));
        saveMeteorologicalStations(configuration);
    }
    
    // Handle exclusions
    QStringList queries;
    
    if (configurationIds.isEmpty()) {
        queries << "delete from meteorological_configuration";
        queries << "delete from meteorological_station";
    } else {
        QString configurationIdsStr = configurationIds.join(",");
        
        queries << "delete from meteorological_configuration where id not in (" + configurationIdsStr + ")";
        queries << "delete from meteorological_station where meteorological_configuration_id not in (" + configurationIdsStr + ")";
    }
    
    for (QString sql : queries) {
        query.prepare(sql);
        query.exec();
    }
}

void ProjectRepository::saveMeteorologicalStations(MeteorologicalConfiguration *configuration) {
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList stationIds;
    
    for (MeteorologicalStation *station : configuration->getStations()) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && station->isPersisted();
        
        if (update) {
            query.prepare("update meteorological_station set name = :n, use_latitude_longitude = :u, utm_x = :x, utm_y = :y, latitude = :la, longitude = :lo where id = :i");
            query.bindValue(":i", station->getId());
        } else {
            query.prepare("insert into meteorological_station (name, use_latitude_longitude, utm_x, utm_y, latitude, longitude, meteorological_configuration_id) values (:n, :u, :x, :y, :la, :lo, :m)");
            query.bindValue(":m", configuration->getId());
        }
        
        query.bindValue(":n", station->getName());
        query.bindValue(":u", station->getUseLatitudeLongitude());
        query.bindValue(":x", station->getUtmX());
        query.bindValue(":y", station->getUtmY());
        query.bindValue(":la", station->getLatitude());
        query.bindValue(":lo", station->getLongitude());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save meteorological stations. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            station->setId(query.lastInsertId().toUInt());
        }
        
        stationIds.append(QString::number(station->getId()));
        saveMeteorologicalParameters(station);
    }
    
    // Handle exclusions
    QStringList queries;
    
    if (stationIds.isEmpty()) {
        queries << "delete from meteorological_station";
        queries << "delete from meteorological_parameter";
    } else {
        QString stationIdsStr = stationIds.join(",");
        
        queries << "delete from meteorological_station where id not in (" + stationIdsStr + ") and meteorological_configuration_id = " + configuration->getId();
    }
    
    for (QString sql : queries) {
        query.prepare(sql);
        query.exec();
    }
}

void ProjectRepository::saveMeteorologicalParameters(MeteorologicalStation *station) {
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList parameterIds;
    
    for (MeteorologicalParameter *parameter : station->getParameters()) {
        if (operationCanceled) {
            return;
        }
        
        bool update = !this->makeCopy && parameter->isPersisted();
        
        if (update) {
            query.prepare("update meteorological_parameter set function = :f, constant_value = :c, use_xy_component = :uxy, x_component = :x, y_component = :y, intensity = :in, direction = :d where id = :i");
            query.bindValue(":i", parameter->getId());
        } else {
            query.prepare("insert into meteorological_parameter (name, unit, function, constant_value, use_xy_component, x_component, y_component, intensity, direction, meteorological_station_id) values (:n, :u, :f, :c, :uxy, :x, :y, :in, :d, :i)");
            query.bindValue(":n", parameter->getName());
            query.bindValue(":u", parameter->getUnit());
            query.bindValue(":i", station->getId());
        }
        
        query.bindValue(":f", (int) parameter->getFunction());
        query.bindValue(":c", parameter->getConstantValue());
        query.bindValue(":uxy", parameter->getUseXYComponent());
        query.bindValue(":x", parameter->getXComponent());
        query.bindValue(":y", parameter->getYComponent());
        query.bindValue(":in", parameter->getIntensity());
        query.bindValue(":d", parameter->getDirection());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save meteorological parameters. Error: %1.").arg(query.lastError().text()).toStdString());
        }
        
        updateProgressAndProcessEvents();
        
        if (!update) {
            parameter->setId(query.lastInsertId().toUInt());
        }
        
        parameterIds.append(QString::number(parameter->getId()));
        saveTimeSeries(parameter);
    }
    
    // Handle exclusions
    QStringList queries;
    
    if (parameterIds.isEmpty()) {
        queries << "delete from meteorological_parameter";
    } else {
        queries << "delete from meteorological_parameter where id not in (" + parameterIds.join(",") + ")";
    }
    
    for (QString sql : queries) {
        query.prepare(sql);
        query.exec();
    }
}

void ProjectRepository::saveSimulations(Project *project) {
    QList<Simulation*> simulations = project->getSimulations();
    QStringList simulationIds;
    
    if (!simulations.isEmpty()) {
        emit updateProgressText("Saving simulations...");
        QApplication::processEvents();
    }
    
    for (Simulation *simulation : simulations) {
        if (operationCanceled) {
            return;
        }
        
        saveSimulation(simulation);
        simulationIds.append(QString::number(simulation->getId()));
    }
    
    // Handle exclusions
    QString simulationDeleteSql;
    
    if (simulationIds.isEmpty()) {
        simulationDeleteSql = "delete from simulation";
    } else {
        QString simulationIdsStr = simulationIds.join(",");
        
        simulationDeleteSql = "delete from simulation where id not in (" + simulationIdsStr + ")";
    }
    
    QSqlQuery query(databaseUtility->getDatabase());
    query.exec(simulationDeleteSql);
}

void ProjectRepository::saveSimulation(Simulation *simulation) {
    QSqlQuery query(databaseUtility->getDatabase());
    QString sql;
    
    bool update = !this->makeCopy && simulation->isPersisted();
    
    if (update) {
        query.prepare("update simulation set label = :l, observations = :o where id = :i");
        query.bindValue(":i", simulation->getId());
    } else {
        query.prepare("insert into simulation (label, simulation_type, start_time, initial_time, period, step_time, minimum_vertical_limit, maximum_vertical_limit, layers, observations, output_time_interval, output_directory, autosave_time_interval, output_parameters, status, recovery_variables, hydrodynamic_configuration_id, meteorological_configuration_id) values (:l, :t, :st1, :it, :p, :st2, :min, :max, :la, :o, :oti, :od, :ati, :op, :s, :r, :h, :m)");
    }
    
    query.bindValue(":l", simulation->getLabel());
    query.bindValue(":t", (int) simulation->getSimulationType());
    query.bindValue(":st1", simulation->getStartTime());
    query.bindValue(":it", simulation->getInitialTime());
    query.bindValue(":p", simulation->getPeriod());
    query.bindValue(":st2", simulation->getStepTime());
    query.bindValue(":min", simulation->getMinimumVerticalLimit());
    query.bindValue(":max", simulation->getMaximumVerticalLimit());
    query.bindValue(":la", simulation->getLayersAsString());
    query.bindValue(":o", simulation->getObservations());
    query.bindValue(":oti", simulation->getOutputTimeInterval());
    query.bindValue(":od", simulation->getOutputDirectory());
    query.bindValue(":ati", simulation->getAutosaveTimeInterval());
    query.bindValue(":op", simulation->getOutputParameters().join(","));
    query.bindValue(":s", (int) simulation->getStatus());
    query.bindValue(":r", simulation->getRecoveryVariables());
    query.bindValue(":h", simulation->getHydrodynamicConfiguration()->getId());
    query.bindValue(":m", simulation->getMeteorologicalConfiguration()->getId());
    
    if (!query.exec()) {
        throw DatabaseException(QString("Unable to save simulations. Error: %1.").arg(query.lastError().text()).toStdString());
    }
    
    updateProgressAndProcessEvents();
    
    if (!update) {
        simulation->setId(query.lastInsertId().toUInt());
    }
}

int ProjectRepository::getMaximumSaveProgress() {
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();
    int saveSteps = 1; // Project
    
    saveSteps += meshes.size();
    
    for (Mesh *mesh : meshes) {
        saveSteps += 1 + mesh->getIslands().size() + mesh->getRefinementAreas().size(); // plus one for boundary polygon
    }
    
    QSet<GridDataConfiguration*> gridDataConfigurations = project->getGridDataConfigurations();
    
    saveSteps += gridDataConfigurations.size();
    
    for (GridDataConfiguration *configuration : gridDataConfigurations) {
        saveSteps += configuration->getGridDataVector().size();
    }
    
    QSet<HydrodynamicConfiguration*> hydrodynamicConfigurations = project->getHydrodynamicConfigurations();
    
    saveSteps += hydrodynamicConfigurations.size();
    
    for (HydrodynamicConfiguration *configuration : hydrodynamicConfigurations) {
        saveSteps += configuration->getParameters().size();
        saveSteps += configuration->getBoundaryConditions().size();
        
        for (BoundaryCondition *boundaryCondition : configuration->getBoundaryConditions()) {
            if (boundaryCondition->getFunction() == BoundaryConditionFunction::TIME_SERIES && (makeCopy || boundaryCondition->isTimeSeriesChanged())) {
                saveSteps += boundaryCondition->getTimeSeriesListPointer()->size();
            }
        }
    }
    
    QSet<MeteorologicalConfiguration*> meteorologicalConfigurations = project->getMeteorologicalConfigurations();
    
    saveSteps += meteorologicalConfigurations.size();
    
    for (MeteorologicalConfiguration *configuration : meteorologicalConfigurations) {
        QList<MeteorologicalStation*> stations = configuration->getStations();

        saveSteps += stations.size();
        
        for (MeteorologicalStation *station : stations) {
            QList<MeteorologicalParameter*> parameters = station->getParameters();
            
            saveSteps += parameters.size();
            
            for (MeteorologicalParameter *parameter : parameters) {
                if (parameter->getFunction() == MeteorologicalParameterFunction::TIME_SERIES && (makeCopy || parameter->isTimeSeriesChanged())) {
                    saveSteps += parameter->getTimeSeriesListPointer()->size();
                }
            }
        }
    }
    
    saveSteps += project->getSimulations().size();
    
    return saveSteps;
}

int ProjectRepository::getMaximumLoadProgress() {
    databaseUtility->connect(this->databaseName);
    
    int loadSteps = 0;
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList tables = {
        "mesh", "mesh_polygon", "grid_data_configuration", "grid_data", "hydrodynamic_configuration", "hydrodynamic_parameter",
        "boundary_condition", "time_series", "meteorological_configuration", "meteorological_station", "meteorological_parameter",
        "simulation"
    };
    
    for (QString table : tables) {
        query.exec("select count(*) from " + table);
        query.next();
        loadSteps += query.value(0).toInt();
    }
    
    return loadSteps;
}

void ProjectRepository::cancelOperation() {
    this->operationCanceled = true;
}