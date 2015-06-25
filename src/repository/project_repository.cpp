#include "include/repository/project_repository.h"

#include "include/exceptions/database_exception.h"
#include "include/utility/database_utility.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>

ProjectRepository::ProjectRepository(const QString &databaseName) : databaseName(databaseName), currentProgress(0), operationCanceled(false) {}

void ProjectRepository::open() {
    currentProgress = 0;
    emit updateProgressText("Loading project...");
    emit updateProgress(currentProgress++);
    QApplication::processEvents();
    
    this->database = DatabaseUtility::connect(this->databaseName);
    
    if (!DatabaseUtility::isDatabaseValid(this->database)) {
        throw DatabaseException("Invalid IPH-ECO Project file.");
    }
    
    QSqlQuery query(this->database);

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
    
    if (!operationCanceled) {
        IPHApplication::setCurrentProject(project);
    }
}

void ProjectRepository::loadMeshes(Project *project) {
    QSqlQuery query(this->database);
    
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
    QSqlQuery query(this->database);
    
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
    QSqlQuery query(this->database);
    
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
    QSqlQuery query(this->database);
    
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
        
        emit updateProgress(currentProgress++);
        QApplication::processEvents();
    }
}

void ProjectRepository::save(bool makeCopy) {
    currentProgress = 0;
    emit updateProgressText("Saving project...");
    emit updateProgress(currentProgress++);
    QApplication::processEvents();
    
    this->database = DatabaseUtility::connect(this->databaseName);
    Project *project = IPHApplication::getCurrentProject();
    QString sql;
    
    QSqlDatabase::database().transaction();
    try {
        if (project->isPersisted() && !makeCopy) {
            sql = "update project set name = :n, description = :d, hydrodynamic = :h, water_quality = :w, sediment = :s";
        } else {
            DatabaseUtility::createApplicationTables(this->database);
            sql = "insert into project (name, description, hydrodynamic, water_quality, sediment) values (:n, :d, :h, :w, :s)";
        }
        
        QSqlQuery query(this->database);
        
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
        
        if (operationCanceled) {
            QSqlDatabase::database().rollback();
        } else {
            QSqlDatabase::database().commit();
        }
    } catch (const DatabaseException &e) {
        QSqlDatabase::database().rollback();
        DatabaseUtility::disconnect(this->database);
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
        QSqlQuery query(this->database);
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
    QSqlQuery query(this->database);
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
        QSqlQuery query(this->database);
        
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
    
    QSqlQuery query(this->database);
    
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
        QSqlQuery query(this->database);
        
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
    QSqlQuery query(this->database);
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
        QSqlQuery query(this->database);
        
        if (gridData->isPersisted()) {
            query.prepare("update grid_data set name = :n, input_type = :it, grid_type = :gt, input_poly_data = :ipd, exponent = :e, radius = :r " \
                          "where id = :i");
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
    
    QSqlQuery query(this->database);
    
    query.prepare("delete from grid_data where id not in (" + gridDataIds.join(",") + ") and grid_data_configuration_id = " + QString::number(gridDataConfiguration->getId()));
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
    this->database = DatabaseUtility::connect(this->databaseName);
    
    int loadSteps = 0;
    QSqlQuery query(this->database);
    
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
    
    return loadSteps;
}

void ProjectRepository::cancelOperation() {
    this->operationCanceled = true;
}