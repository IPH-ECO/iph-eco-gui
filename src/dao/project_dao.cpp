#include "include/dao/project_dao.h"

#include "include/exceptions/database_exception.h"
#include "include/utility/database_utility.h"
#include "include/domain/structured_mesh.h"

#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
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
    project->setFilename(this->databaseName);

    return project;
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
    }

    project->setDirty(false);

    DatabaseUtility::disconnect(db);
}

void ProjectDAO::saveMeshes(QSqlDatabase &db, Project *project) {
    QSet<Mesh*> meshes = project->getMeshes();
    
    for (QSet<Mesh*>::const_iterator it = meshes.begin(); it != meshes.end(); it++) {
        Mesh *mesh = *it;
        QString sql;
        
        if (mesh->isPersisted()) {
            sql = "update mesh set name = :n, type = :t, poly_data = :p, coordinates_distance = :c, resolution = :r";
        } else {
            sql = "insert into mesh (name, type, coordinates_distance, poly_data, resolution) values (:n, :t, :c, :p, :r)";
        }
        
        QString meshType = mesh->instanceOf("StructuredMesh") ? "StructuredMesh" : "UnstructuredMesh";
        QSqlQuery query(db);
        
        query.prepare(sql);
        query.bindValue(":n", mesh->getName());
        query.bindValue(":t", meshType);
        query.bindValue(":c", mesh->getCoordinatesDistance());
        query.bindValue(":p", mesh->getGridAsString());
        
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
            sql = "update mesh_polygon set type = :t, poly_data = :p, minimum_angle = :mi, maximum_edge_length = :ma where mesh_id = :me";
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
