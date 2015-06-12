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
    
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); it++) {
        Mesh *mesh = *it;
        QString sql;
        
        if (mesh->isPersisted()) {
            sql = "update mesh set name = :n, type = :t, poly_data = :p, coordinates_distance = :c, resolution = :r where id = " + QString::number(mesh->getId());
        } else {
            sql = "insert into mesh (name, type, coordinates_distance, poly_data, resolution) values (:n, :t, :c, :p, :r)";
        }
        
        QString meshType = mesh->instanceOf("StructuredMesh") ? "StructuredMesh" : "UnstructuredMesh";
        QSqlQuery query(db);
        
        query.prepare(sql);
        query.bindValue(":n", mesh->getName());
        query.bindValue(":t", meshType);
        query.bindValue(":p", mesh->getGridAsString());
        query.bindValue(":c", mesh->getCoordinatesDistance());
        
        if (mesh->instanceOf("StructuredMesh")) {
            query.bindValue(":r", static_cast<StructuredMesh*>(mesh)->getResolution());
        } else {
            query.bindValue(":r", "NULL");
        }
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save meshes. Error: %1.").arg(query.lastError().text()));
        }
        
        mesh->setId(query.lastInsertId().toUInt());
        saveMeshPolygons(db, mesh);
    }
}

void ProjectDAO::saveMeshPolygons(QSqlDatabase &db, Mesh *mesh) {
    QList<MeshPolygon*> meshPolygons = mesh->getIslands() + mesh->getRefinementAreas();
    
    meshPolygons.prepend(mesh->getBoundaryPolygon());
    
    for (QList<MeshPolygon*>::const_iterator it = meshPolygons.begin(); it != meshPolygons.end(); it++) {
        MeshPolygon *meshPolygon = *it;
        QString sql;
        
        if (meshPolygon->isPersisted()) {
            sql = "update mesh_polygon set type = :t, poly_data = :p, minimum_angle = :mi, maximum_edge_length = :ma where mesh_id = :me and id = " + QString::number(meshPolygon->getId());
        } else {
            sql = "insert into mesh_polygon (type, poly_data, minimum_angle, maximum_edge_length, mesh_id) values (:t, :p, :mi, :ma, :me)";
        }
        
        QSqlQuery query(db);
        
        query.prepare(sql);
        query.bindValue(":t", (int) meshPolygon->getMeshPolygonType());
        query.bindValue(":p", meshPolygon->getPolyDataAsString());
        if (mesh->instanceOf("UnstructuredMesh")) {
            query.bindValue(":mi", meshPolygon->getMinimumAngle());
            query.bindValue(":ma", meshPolygon->getMaximumEdgeLength());
        } else {
            query.bindValue(":mi", "NULL");
            query.bindValue(":ma", "NULL");
        }
        query.bindValue(":me", mesh->getId());
        
        if (!query.exec()) {
            throw DatabaseException(QString("Unable to save project meshes. Error: %1.").arg(query.lastError().text()));
        }
        
        meshPolygon->setId(query.lastInsertId().toUInt());
    }
}
