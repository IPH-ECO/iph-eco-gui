#include "include/repository/project_repository.h"

#include "include/application/iph_application.h"
#include "include/exceptions/database_exception.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>

ProjectRepository::ProjectRepository(const QString &databaseName) : databaseName(databaseName), currentProgress(0), operationCanceled(false) {
    databaseUtility = DatabaseUtility::getInstance();
}

void ProjectRepository::open() {
    currentProgress = 0;
    emit updateProgressText("Loading project...");
    emit updateProgress(currentProgress++);
    QApplication::processEvents();
    
    databaseUtility->connect(this->databaseName);
    
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
    bool sediment = query.value("sediment").toBool();
    bool waterQuality = query.value("water_quality").toBool();

    Project *project = new Project(name, description, hydrodynamic, sediment, waterQuality);
    project->setId(query.value("id").toUInt());
    project->setFilename(this->databaseName);
    
    loadMeshes(project);
    loadGridDataConfigurations(project);
    loadHydrodynamicConfigurations(project);
    
    if (!operationCanceled) {
        IPHApplication::setCurrentProject(project);
    }
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
        mesh->loadMeshPolygonsFromStringPolyData(query.value("poly_data").toString());
        project->addMesh(mesh);
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
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
        meshPolygon->loadPolygonsFromStringPolyData(query.value("poly_data").toString());
        meshPolygon->setMinimumAngle(query.value("minimum_angle").toDouble());
        meshPolygon->setMaximumEdgeLength(query.value("maximum_edge_length").toDouble());
        
        mesh->addMeshPolygon(meshPolygon);
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
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
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
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

        gridData->setMapMinimumRange(query.value("map_minimum_range").toDouble());
        gridData->setMapMaximumRange(query.value("map_maximum_range").toDouble());
        gridData->setMapColorGradient(query.value("map_color_gradient").toString());
        gridData->setMapInvertColorGradient(query.value("map_invert_color_gradient").toBool());
        gridData->setMapOpacity(query.value("map_opacity").toInt());
        gridData->setMapLighting(query.value("map_lighting").toBool());
        gridData->setMapLegend(query.value("map_legend").toBool());
        
        gridData->setPointsMinimumRange(query.value("points_minimum_range").toDouble());
        gridData->setPointsMaximumRange(query.value("points_maximum_range").toDouble());
        gridData->setPointsColorGradient(query.value("points_color_gradient").toString());
        gridData->setPointsInvertColorGradient(query.value("points_invert_color_gradient").toBool());
        gridData->setPointsOpacity(query.value("points_opacity").toInt());
        gridData->setPointsSize(query.value("points_size").toInt());
        gridData->setPointsLegend(query.value("points_legend").toBool());

        gridData->setMeshLineColor(query.value("mesh_line_color").toString());
        gridData->setMeshLineStyle(query.value("mesh_line_style").toInt());
        gridData->setMeshLineWidth(query.value("mesh_line_width").toInt());
        gridData->setMeshOpacity(query.value("mesh_opacity").toInt());

        gridData->setGridDataConfiguration(gridDataConfiguration);
        
        gridDataConfiguration->addGridData(gridData);
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
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
        configuration->setId(query.value("id").toUInt());
        configuration->setName(query.value("name").toString());
        project->addHydrodynamicConfiguration(configuration);
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
        loadHydrodynamicParameter(configuration, project);
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
        parameter->setValue(query.value("value").toDouble());
        parameter->setSelected(query.value("selected").toDouble());

        configuration->addHydrodynamicParameter(parameter);

        emit updateProgress(currentProgress++);
        QApplication::processEvents();
    }
}

void ProjectRepository::save(bool makeCopy) {
    currentProgress = 0;
    emit updateProgressText("Saving project...");
    emit updateProgress(currentProgress++);
    QApplication::processEvents();
    
    Project *project = IPHApplication::getCurrentProject();
    QString sql;
    
    databaseUtility->connect(project->getFilename());
    
    QSqlDatabase::database().transaction();
    try {
        if (project->isPersisted() && !makeCopy) {
            sql = "update project set name = :n, description = :d, hydrodynamic = :h, water_quality = :w, sediment = :s";
        } else {
            databaseUtility->createApplicationTables();
            sql = "insert into project (name, description, hydrodynamic, water_quality, sediment) values (:n, :d, :h, :w, :s)";
        }
        
        QSqlQuery query(databaseUtility->getDatabase());
        
        query.prepare(sql);
        query.bindValue(":n", project->getName());
        query.bindValue(":d", project->getDescription());
        query.bindValue(":h", project->getHydrodynamic());
        query.bindValue(":w", project->getWaterQuality());
        query.bindValue(":s", project->getSediment());

        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save project. Error: %1.").arg(query.lastError().text()));
        }
        
        project->setId(1);
        saveMeshes(project);
        saveGridDataConfigurations(project);
        saveHydrodynamicConfigurations(project);
        
        if (operationCanceled) {
            QSqlDatabase::database().rollback();
        } else {
            QSqlDatabase::database().commit();
        }
    } catch (const DatabaseException &e) {
        QSqlDatabase::database().rollback();
        databaseUtility->disconnect();
        throw e;
    } catch (const std::exception &e) {
        throw DatabaseException(e.what());
    }

    project->setDirty(false);
}

void ProjectRepository::saveMeshes(Project *project) {
    QSet<Mesh*> meshes = project->getMeshes();
    QStringList meshIds;
    
    if (!meshes.isEmpty()) {
        emit updateProgressText("Saving meshes...");
        QApplication::processEvents();
    }
    
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end() && !operationCanceled; it++) {
        QSqlQuery query(databaseUtility->getDatabase());
        Mesh *mesh = *it;
        QString sql;
        
        if (mesh->isPersisted()) {
            query.prepare("update mesh set name = :n, type = :t, poly_data = :p, coordinates_distance = :c, resolution = :r where id = :m");
            query.bindValue(":m", mesh->getId());
        } else {
            query.prepare("insert into mesh (name, type, coordinates_distance, poly_data, resolution) values (:n, :t, :c, :p, :r)");
        }
        
        QString meshType = mesh->instanceOf("StructuredMesh") ? "StructuredMesh" : "UnstructuredMesh";
        
        query.bindValue(":n", mesh->getName());
        query.bindValue(":t", meshType);
        query.bindValue(":c", mesh->getCoordinatesDistance());
        query.bindValue(":p", mesh->getPolyDataAsString());
        
        if (mesh->instanceOf("StructuredMesh")) {
            query.bindValue(":r", static_cast<StructuredMesh*>(mesh)->getResolution());
        } else {
            query.bindValue(":r", "NULL");
        }
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save meshes. Error: %1.").arg(query.lastError().text()));
        }
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
        mesh->setId(query.lastInsertId().toUInt());
        meshIds.append(QString::number(mesh->getId()));
        saveMeshPolygons(mesh);
    }
    
    if (operationCanceled) {
        return;
    }
    
    // Handle exclusions
    QSqlQuery query(databaseUtility->getDatabase());
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
    QStringList meshPolygonIds;
    
    meshPolygons.prepend(mesh->getBoundaryPolygon());
    
    for (QList<MeshPolygon*>::const_iterator it = meshPolygons.begin(); it != meshPolygons.end() && !operationCanceled; it++) {
        MeshPolygon *meshPolygon = *it;
        QSqlQuery query(databaseUtility->getDatabase());
        
        if (meshPolygon->isPersisted()) {
            query.prepare("update mesh_polygon set name = :n, type = :t, poly_data = :p, minimum_angle = :mi, maximum_edge_length = :ma where id = :i");
            query.bindValue(":i", meshPolygon->getId());
        } else {
            query.prepare("insert into mesh_polygon (name, type, poly_data, minimum_angle, maximum_edge_length, mesh_id) values (:n, :t, :p, :mi, :ma, :me)");
            query.bindValue(":me", mesh->getId());
        }
        
        query.bindValue(":n", meshPolygon->getName());
        query.bindValue(":t", (int) meshPolygon->getMeshPolygonType());
        query.bindValue(":p", meshPolygon->getPolyDataAsString());
        if (mesh->instanceOf("UnstructuredMesh")) {
            query.bindValue(":mi", meshPolygon->getMinimumAngle());
            query.bindValue(":ma", meshPolygon->getMaximumEdgeLength());
        } else {
            query.bindValue(":mi", "NULL");
            query.bindValue(":ma", "NULL");
        }
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save project meshes. Error: %1.").arg(query.lastError().text()));
        }
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
        meshPolygon->setId(query.lastInsertId().toUInt());
        meshPolygonIds.append(QString::number(meshPolygon->getId()));
    }
    
    if (operationCanceled) {
        return;
    }
    
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("delete from mesh_polygon where id not in (" + meshPolygonIds.join(",") + ") and mesh_id = " + QString::number(mesh->getId()));
    query.exec();
}

void ProjectRepository::saveGridDataConfigurations(Project *project) {
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();
    QStringList configurationIds;
    
    if (!configurations.isEmpty()) {
        emit updateProgressText("Saving grid data...");
        QApplication::processEvents();
    }
    
    for (QSet<GridDataConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end() && !operationCanceled; it++) {
        GridDataConfiguration *configuration = *it;
        QSqlQuery query(databaseUtility->getDatabase());
        
        if (configuration->isPersisted()) {
            query.prepare("update grid_data_configuration set name = :n where id = :i");
            query.bindValue(":i", configuration->getId());
        } else {
            query.prepare("insert into grid_data_configuration (name) values (:n)");
        }
        
        query.bindValue(":n", configuration->getName());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save grid data configurations. Error: %1.").arg(query.lastError().text()));
        }
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
        configuration->setId(query.lastInsertId().toUInt());
        configurationIds.append(QString::number(configuration->getId()));
        saveGridData(configuration);
    }
    
    if (operationCanceled) {
        return;
    }
    
    // Handle exclusions
    QSqlQuery query(databaseUtility->getDatabase());
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
    QStringList gridDataIds;
    
    for (int i = 0; i < gridDataVector.size() && !operationCanceled; i++) {
        GridData *gridData = gridDataVector.at(i);
        QSqlQuery query(databaseUtility->getDatabase());
        
        if (gridData->isPersisted()) {
            query.prepare("update grid_data set " \
                          "name = :n, input_type = :it, grid_type = :gt, input_poly_data = :ipd, exponent = :e, radius = :r, " \
                          "map_minimum_range = :mminr, map_maximum_range = :mmaxr, map_color_gradient = :mcg, map_invert_color_gradient = :micg, map_opacity = :mop, map_lighting = :mli, map_legend = :mle, " \
                          "points_minimum_range = :pminr, points_maximum_range = :pmaxr, points_color_gradient = :pcg, points_invert_color_gradient = :picg, points_opacity = :pop, points_size = :psz, points_legend = :ple, " \
                          "mesh_line_color = :mlc, mesh_line_style = :mls, mesh_line_width = :mlw, mesh_opacity = :meop " \
                          "where id = :i");
            query.bindValue(":i", gridData->getId());
        } else {
            query.prepare("insert into grid_data (" \
                          "name, input_type, grid_type, input_poly_data, exponent, radius, " \
                          "map_minimum_range, map_maximum_range, map_color_gradient, map_invert_color_gradient, map_opacity, map_lighting, map_legend, " \
                          "points_minimum_range, points_maximum_range, points_color_gradient, points_invert_color_gradient, points_opacity, points_size, points_legend, " \
                          "mesh_line_color, mesh_line_style, mesh_line_width, mesh_opacity, " \
                          "grid_data_configuration_id, mesh_id) " \
                          "values (:n, :it, :gt, :ipd, :e, :r, :mminr, :mmaxr, :mcg, :micg, :mop, :mli, :mle, :pminr, :pmaxr, :pcg, :picg, :pop, :psz, :ple, :mlc, :mls, :mlw, :meop, :gdc, :m)");
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
        
        query.bindValue(":mminr", gridData->getMapMininumRange());
        query.bindValue(":mmaxr", gridData->getMapMaximumRange());
        query.bindValue(":mcg", gridData->getMapColorGradient());
        query.bindValue(":micg", gridData->getMapInvertColorGradient());
        query.bindValue(":mop", gridData->getMapOpacity());
        query.bindValue(":mli", gridData->getMapLighting());
        query.bindValue(":mle", gridData->getMapLegend());

        query.bindValue(":pminr", gridData->getPointsMininumRange());
        query.bindValue(":pmaxr", gridData->getPointsMaximumRange());
        query.bindValue(":pcg", gridData->getPointsColorGradient());
        query.bindValue(":picg", gridData->getPointsInvertColorGradient());
        query.bindValue(":pop", gridData->getPointsOpacity());
        query.bindValue(":psz", gridData->getPointsSize());
        query.bindValue(":ple", gridData->getPointsLegend());

        query.bindValue(":mlc", gridData->getMeshLineColor());
        query.bindValue(":mls", gridData->getMeshLineStyle());
        query.bindValue(":mlw", gridData->getMeshLineWidth());
        query.bindValue(":meop", gridData->getMeshOpacity());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save grid data configurations. Error: %1.").arg(query.lastError().text()));
        }
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
        gridData->setId(query.lastInsertId().toUInt());
        gridDataIds.append(QString::number(gridData->getId()));
    }
    
    if (operationCanceled) {
        return;
    }
    
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.prepare("delete from grid_data where id not in (" + gridDataIds.join(",") + ") and grid_data_configuration_id = " + QString::number(gridDataConfiguration->getId()));
    query.exec();
}

void ProjectRepository::saveHydrodynamicConfigurations(Project *project) {
    QSet<HydrodynamicConfiguration*> configurations = project->getHydrodynamicConfigurations();
    QStringList configurationIds;
    
    if (!configurations.isEmpty()) {
        emit updateProgressText("Saving hydrodynamic data...");
        QApplication::processEvents();
    }
    
    for (QSet<HydrodynamicConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end() && !operationCanceled; it++) {
        HydrodynamicConfiguration *configuration = *it;
        QSqlQuery query(databaseUtility->getDatabase());
        
        if (configuration->isPersisted()) {
            query.prepare("update hydrodynamic_configuration set name = :n where id = :i");
            query.bindValue(":i", configuration->getId());
        } else {
            query.prepare("insert into hydrodynamic_configuration (name) values (:n)");
        }
        
        query.bindValue(":n", configuration->getName());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save hydrodynamic data configurations. Error: %1.").arg(query.lastError().text()));
        }
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
        
        configuration->setId(query.lastInsertId().toUInt());
        configurationIds.append(QString::number(configuration->getId()));
        saveHydrodynamicParameters(configuration);
    }
}

void ProjectRepository::saveHydrodynamicParameters(HydrodynamicConfiguration *configuration) {
    QList<HydrodynamicParameter*> parameters = configuration->getParameters();
    QSqlQuery query(databaseUtility->getDatabase());
    QStringList parameterIds;

    for (int i = 0; i < parameters.size() && !operationCanceled; i++) {
        HydrodynamicParameter *parameter = parameters[i];

        if (parameter->isPersisted()) {
            query.prepare("update hydrodynamic_parameter set value = :v, selected = :s where id = :i");
            query.bindValue(":i", parameter->getId());
        } else {
            query.prepare("insert into hydrodynamic_parameter (name, value, selected, hydrodynamic_configuration_id) values (:n, :v, :s, :h)");
            query.bindValue(":n", parameter->getName());
            query.bindValue(":h", configuration->getId());
        }

        query.bindValue(":v", parameter->getValue());
        query.bindValue(":s", parameter->isSelected());

        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save hydrodynamic parameters. Error: %1.").arg(query.lastError().text()));
        }

        emit updateProgress(currentProgress++);
        QApplication::processEvents();

        parameter->setId(query.lastInsertId().toUInt());
        parameterIds.append(QString::number(parameter->getId()));
    }

    if (operationCanceled) {
        return;
    }

    query.prepare("delete from hydrodynamic_parameter where id not in (" + parameterIds.join(",") + ") and hydrodynamic_configuration_id = " + configuration->getId());
    query.exec();
}

int ProjectRepository::getMaximumSaveProgress() {
    int saveSteps = 0;
    Project *project = IPHApplication::getCurrentProject();
    QSet<Mesh*> meshes = project->getMeshes();
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();
    
    saveSteps += meshes.size();
    
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); it++) {
        saveSteps += 1 + (*it)->getIslands().size() + (*it)->getRefinementAreas().size(); // plus one for boundary polygon
    }
    
    saveSteps += configurations.size();
    
    for (QSet<GridDataConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end(); it++) {
        saveSteps += (*it)->getGridDataVector().size();
    }
    
    return saveSteps;
}

int ProjectRepository::getMaximumLoadProgress() {
    databaseUtility->connect(this->databaseName);
    
    int loadSteps = 0;
    QSqlQuery query(databaseUtility->getDatabase());
    
    query.exec("select count(*) from mesh");
    query.next();
    loadSteps += query.value(0).toInt();
    
    query.exec("select count(*) from mesh_polygon");
    query.next();
    loadSteps += query.value(0).toInt();
    
    query.exec("select count(*) from grid_data_configuration");
    query.next();
    loadSteps += query.value(0).toInt();
    
    query.exec("select count(*) from grid_data");
    query.next();
    loadSteps += query.value(0).toInt();

    query.exec("select count(*) from hydrodynamic_configuration");
    query.next();
    loadSteps += query.value(0).toInt();

    query.exec("select count(*) from hydrodynamic_parameter");
    query.next();
    loadSteps += query.value(0).toInt();
    
    return loadSteps;
}

void ProjectRepository::cancelOperation() {
    this->operationCanceled = true;
}