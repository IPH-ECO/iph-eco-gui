#include "include/dao/project_dao.h"

#include "include/exceptions/database_exception.h"
#include "include/utility/database_utility.h"
#include "include/domain/structured_mesh.h"
#include "include/domain/unstructured_mesh.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSet>

ProjectDAO::ProjectDAO(const QString &_databaseName) : databaseName(_databaseName) {}

Project* ProjectDAO::open() {
    QSqlDatabase db = DatabaseUtility::connect(this->databaseName);

    if (!DatabaseUtility::isDatabaseValid(db)) {
        throw DatabaseException("Invalid IPH-ECO Project file.");
    }

    QSqlQuery query(db);

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
    
    loadMeshes(db, project);

    return project;
}

void ProjectDAO::loadMeshes(QSqlDatabase &db, Project *project) {
    QSqlQuery query(db);
    
    query.prepare("select * from mesh");
    query.exec();
    
    while (query.next()) {
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
        
        loadMeshPolygons(db, mesh);
    }
}

void ProjectDAO::loadMeshPolygons(QSqlDatabase &db, Mesh *mesh) {
    QSqlQuery query(db);
    
    query.prepare("select * from mesh_polygon where mesh_id = :m");
    query.bindValue(":m", mesh->getId());
    query.exec();
    
    while (query.next()) {
        MeshPolygon *meshPolygon = new MeshPolygon();
        meshPolygon->setId(query.value("id").toUInt());
        meshPolygon->setMeshPolygonType(static_cast<MeshPolygonType>(query.value("type").toInt()));
        meshPolygon->loadPolygonsFromStringPolyData(query.value("poly_data").toString());
        if (mesh->instanceOf("UnstructuredMesh")) {
            meshPolygon->setMinimumAngle(query.value("minimum_angle").toDouble());
            meshPolygon->setMaximumEdgeLength(query.value("maximum_edge_length").toDouble());
        }
        
        mesh->addMeshPolygon(meshPolygon);
    }
}

void ProjectDAO::save(Project *project) {
    QSqlDatabase db = DatabaseUtility::connect(this->databaseName);
    QString sql;
    
    QSqlDatabase::database().transaction();
    try {
        if (project->isPersisted()) {
            sql = "update project set name = :n, description = :d, hydrodynamic = :h, water_quality = :w, sediment = :s";
        } else {
            DatabaseUtility::createApplicationTables(db);
            sql = "insert into project (name, description, hydrodynamic, water_quality, sediment) values (:n, :d, :h, :w, :s)";
        }
        
        QSqlQuery query(db);
        
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
        saveMeshes(db, project);
        QSqlDatabase::database().commit();
    } catch (const DatabaseException &e) {
        QSqlDatabase::database().rollback();
        DatabaseUtility::disconnect(db);
        throw e;
    } catch (const std::exception &e) {
        throw DatabaseException(e.what());
    }

    project->setDirty(false);
}

void ProjectDAO::saveMeshes(QSqlDatabase &db, Project *project) {
    QSet<Mesh*> meshes = project->getMeshes();
    QStringList meshIds;
    
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); it++) {
        Mesh *mesh = *it;
        QSqlQuery query(db);
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
        
        mesh->setId(query.lastInsertId().toUInt());
        meshIds.append(QString::number(mesh->getId()));
        saveMeshPolygons(db, mesh);
    }
    
    // Handle exclusions
    QSqlQuery query(db);
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

void ProjectDAO::saveMeshPolygons(QSqlDatabase &db, Mesh *mesh) {
    QList<MeshPolygon*> meshPolygons = mesh->getIslands() + mesh->getRefinementAreas();
    QStringList meshPolygonIds;
    
    meshPolygons.prepend(mesh->getBoundaryPolygon());
    
    for (QList<MeshPolygon*>::const_iterator it = meshPolygons.begin(); it != meshPolygons.end(); it++) {
        MeshPolygon *meshPolygon = *it;
        QSqlQuery query(db);
        
        if (meshPolygon->isPersisted()) {
            query.prepare("update mesh_polygon set type = :t, poly_data = :p, minimum_angle = :mi, maximum_edge_length = :ma where id = :i");
            query.bindValue(":i", meshPolygon->getId());
        } else {
            query.prepare("insert into mesh_polygon (type, poly_data, minimum_angle, maximum_edge_length, mesh_id) values (:t, :p, :mi, :ma, :me)");
            query.bindValue(":me", mesh->getId());
        }
        
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
        
        meshPolygon->setId(query.lastInsertId().toUInt());
        meshPolygonIds.append(QString::number(meshPolygon->getId()));
    }
    
    QSqlQuery query(db);
    
    query.prepare("delete from mesh_polygon where id not in (" + meshPolygonIds.join(",") + ") and mesh_id = " + QString::number(mesh->getId()));
    query.exec();
}

void ProjectDAO::saveGridDataConfigurations(QSqlDatabase &db, Project *project) {
    QSet<GridDataConfiguration*> configurations = project->getGridDataConfigurations();
    QStringList configurationIds;
    
    for (QSet<GridDataConfiguration*>::const_iterator it = configurations.begin(); it != configurations.end(); it++) {
        GridDataConfiguration *configuration = *it;
        QSqlQuery query(db);
        
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
        
        configuration->setId(query.lastInsertId().toUInt());
        configurationIds.append(QString::number(configuration->getId()));
    }
    
    // Handle exclusions
    QSqlQuery query(db);
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

void ProjectDAO::saveGridData(QSqlDatabase &db, GridDataConfiguration *gridDataConfiguration) {
    QVector<GridData*> gridDataVector = gridDataConfiguration->getGridDataVector();
    QStringList gridDataIds;
    
    for (int i = 0; i < gridDataVector.size(); i++) {
        GridData *gridData = gridDataVector.at(i);
        QSqlQuery query(db);
        
        if (gridData->isPersisted()) {
            query.prepare("update grid_data set name = :n, input_type = :it, grid_type = :gt, input_poly_data = :ipd, exponent = :e, radius = :r " \
                          "where id = :i");
            query.bindValue(":i", gridData->getId());
        } else {
            query.prepare("insert into grid_data values (name, input_type, grid_type, input_poly_data, exponent, radius, grid_data_configuration_id, mesh_id) " \
                          "values (:n, :it, :gt, :ipd, :e, :r, :gdc, :m)");
            query.bindValue(":gdc", gridDataConfiguration->getId());
            query.bindValue(":m", gridData->getMesh()->getId());
        }
        
        query.bindValue(":n", gridData->getName());
        query.bindValue(":it", (int) gridData->getGridDataInputType());
        query.bindValue(":gt", (int) gridData->getGridDataType());
        query.bindValue(":ipd", gridData->getInputPolyDataAsString());
        query.bindValue(":e", gridData->getExponent());
        query.bindValue(":r", gridData->getRadius());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save grid data configurations. Error: %1.").arg(query.lastError().text()));
        }
        
        gridData->setId(query.lastInsertId().toUInt());
        gridDataIds.append(QString::number(gridData->getId()));
    }
    
    QSqlQuery query(db);
    
    query.prepare("delete from grid_data where id not in (" + gridDataIds.join(",") + ") and grid_data_configuration_id = " + QString::number(gridDataConfiguration->getId()));
    query.exec();
}