#include "include/utility/database_utility.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QFile>
#include <QList>

QSqlDatabase DatabaseUtility::connect(QString &database_name) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
    db.setDatabaseName(database_name);

    if (!db.open()) {
        throw DatabaseException(QString("The following error ocurred during database connection: %1").arg(db.lastError().text()));
    }

    return db;
}

void DatabaseUtility::disconnect(QSqlDatabase &database) {
    if (database.isOpen()) {
        database.close();
    }
    QSqlDatabase::removeDatabase(database.connectionName());
}

void DatabaseUtility::createApplicationTables(QSqlDatabase &database) {
    QStringList sql;

    sql << "create table if not exists project (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "description text, " \
        "hydrodynamic boolean default false," \
        "water_quality boolean default false," \
        "sediment boolean default false" \
    ")";

    sql << "create table if not exists mesh (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "coordinates_distance float default 0, " \
        "poly_data text not null, " \
        "resolution integer" \
    ")";

    sql << "create table if not exists mesh_polygon (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "poly_data text not null, " \
        "minimum_angle float, " \
        "maximum_edge_length float, " \
        "mesh_id integer not null" \
    ")";
    
    sql << "create table if not exists grid_data_configuration (" \
        "id integer primary key, " \
        "name varchar(255) not null" \
    ")";
    
    sql << "create table if not exists grid_data (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "input_type integer not null, " \
        "grid_type integer not null, " \
        "input_poly_data text not null, " \
        "exponent float, " \
        "radius float, " \

        "map_minimum_range float default 0, " \
        "map_maximum_range float default 0, " \
        "map_color_gradient varchar(255) default 'Default', " \
        "map_invert_color_gradient bool default false, " \
        "map_opacity int default 100, " \
        "map_lighting bool default false, " \
        "map_legend bool default true, " \

        "points_minimum_range float default 0, " \
        "points_maximum_range float default 0, " \
        "points_color_gradient varchar(255) default 'Default', " \
        "points_invert_color_gradient bool default false, " \
        "points_opacity int default 100, " \
        "points_size int default 1, " \
        "points_legend bool default false, " \

        "mesh_line_color varchar(50) default '#000000', " \
        "mesh_line_style int default x'FFFF', " \
        "mesh_line_width int default 1, " \
        "mesh_opacity int default 100, " \

        "grid_data_configuration_id integer not null, " \
        "mesh_id integer not null" \
    ")";
    
    sql << "create table if not exists hydrodynamic_parameter (" \
        "name varchar(255) primary key, " \
        "value float default null"
    ")";
    
    QSqlQuery query(database);

    for (int i = 0; i < sql.count(); i++) {
        query.prepare(sql.at(i));

        if (!query.exec()) {
            throw DatabaseException(QString("An error occurred when creating application tables: %1").arg(query.lastError().text()));
        }
    }

    query.prepare(QString("pragma application_id = %1").arg(IPHApplication::getApplicationId()));
    query.exec();
}

/*void DatabaseUtility::seedHydrodynamicDataTables(QSqlDatabase &database) {
    QFile seedsFile(":/data/seed/hydrodynamic_data.json");
    
    seedsFile.open(QFile::ReadOnly);
    
    QJsonDocument jsonDocument = QJsonDocument::fromJson(seedsFile.readAll());
    QJsonObject jsonObject = jsonDocument.object();
    QJsonArray jsonParameters = jsonObject["parameters"].toArray();
    
    for (int i = 0; i < jsonParameters.size(); i++) {
        QJsonObject jsonParameter = jsonParameters[i].toObject();
        QString parentName = jsonParameter["parentName"].toString();
        QSqlQuery query(database);
        QString sql = "insert into hydrodynamic_parameter (name, label, parent_name, editable, hide_siblings, default_value, range_minimum, range_maximum) "
                      "values (:na, :la, :pa, :ed, :hs, :dv, :rmin, :rmax)";
        
        query.prepare(sql);
        query.bindValue(":na", jsonParameter["name"].toString());
        query.bindValue(":la", jsonParameter["label"].toString());
        query.bindValue(":pa", parentName.isEmpty() ? QVariant(QVariant::String) : parentName);
        query.bindValue(":ed", jsonParameter["editable"].toBool());
        query.bindValue(":hs", jsonParameter["hideSiblings"].toBool());
        query.bindValue(":dv", jsonParameter["defaultValue"].toDouble());
        query.bindValue(":rmin", jsonParameter["rangeMinimum"].toDouble());
        query.bindValue(":rmax", jsonParameter["rangeMaximumaultValue"].toDouble());
        
        if (!query.exec()) {
            throw DatabaseException(QString("An error occurred when seeding application tables: %1").arg(query.lastError().text()));
        }
    }
    
    QJsonArray jsonProcesses = jsonObject["processes"].toArray();
    
    for (int i = 0; i < jsonProcesses.size(); i++) {
        QJsonObject jsonProcess = jsonProcesses[i].toObject();
        QString parentName = jsonProcess["parentName"].toString();
        QString targetParameter = jsonProcess["targetParameter"].toString();
        QSqlQuery query(database);
        QString sql = "insert into hydrodynamic_process (name, label, parent_name, target_parameter, checkable, checked) "
                      "values (:na, :la, :pa, :ta, :ce, :cd)";
        
        query.prepare(sql);
        query.bindValue(":na", jsonProcess["name"].toString());
        query.bindValue(":la", jsonProcess["label"].toString());
        query.bindValue(":pa", parentName.isEmpty() ? QVariant(QVariant::String) : parentName);
        query.bindValue(":ta", targetParameter.isEmpty() ? QVariant(QVariant::String) : targetParameter);
        query.bindValue(":ce", jsonProcess["checkable"].toBool());
        query.bindValue(":cd", jsonProcess["checked"].toBool());
        
        if (!query.exec()) {
            throw DatabaseException(QString("An error occurred when seeding application tables: %1").arg(query.lastError().text()));
        }
    }
}*/

bool DatabaseUtility::isDatabaseValid(QSqlDatabase &database) {
    QSqlQuery query(database);

    query.prepare("pragma application_id");
    query.exec();
    query.next();

    return query.value(0).toInt() == IPHApplication::getApplicationId();
}
