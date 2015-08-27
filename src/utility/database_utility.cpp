#include "include/utility/database_utility.h"

#include "include/application/iph_application.h"
#include "include/exceptions/database_exception.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QFile>
#include <QList>

DatabaseUtility* DatabaseUtility::instance = nullptr;

DatabaseUtility::DatabaseUtility() : previousDatabase(nullptr) {}

DatabaseUtility* DatabaseUtility::getInstance() {
    if (instance == nullptr) {
        instance = new DatabaseUtility();
    }
    
    return instance;
}

void DatabaseUtility::connect(const QString &databaseName, bool force) {
    if (!currentDatabase.isOpen() || force) {
        if (force) {
            previousDatabase = &currentDatabase;
        }
        
        currentDatabase = QSqlDatabase::addDatabase("QSQLITE", QUuid::createUuid().toString());
        currentDatabase.setDatabaseName(databaseName);

        if (!currentDatabase.open()) {
            revertConnection();
            throw DatabaseException(QString("The following error ocurred during database connection: %1").arg(currentDatabase.lastError().text()));
        }
    }
}

void DatabaseUtility::disconnect() {
    if (currentDatabase.isOpen()) {
        currentDatabase.close();
    }
    QSqlDatabase::removeDatabase(currentDatabase.databaseName());
}

void DatabaseUtility::revertConnection() {
    if (previousDatabase) {
        disconnect();
        currentDatabase = *previousDatabase;
    }
}

void DatabaseUtility::createApplicationTables() {
    QStringList sql;

    sql << "drop table if exists project";
    sql << "create table project (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "description text, " \
        "hydrodynamic boolean default false," \
        "water_quality boolean default false," \
        "sediment boolean default false" \
    ")";

    sql << "drop table if exists mesh";
    sql << "create table mesh (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "coordinates_distance float default 0, " \
        "mesh_poly_data text not null, " \
        "boundary_poly_data text not null, " \
        "resolution integer" \
    ")";

    sql << "drop table if exists mesh_polygon";
    sql << "create table mesh_polygon (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "poly_data text not null, " \
        "minimum_angle float, " \
        "maximum_edge_length float, " \
        "mesh_id integer not null" \
    ")";
    
    sql << "drop table if exists grid_data_configuration";
    sql << "create table grid_data_configuration (" \
        "id integer primary key, " \
        "name varchar(255) not null" \
    ")";
    
    sql << "drop table if exists grid_data";
    sql << "create table grid_data (" \
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
    
    sql << "drop table if exists hydrodynamic_configuration";
    sql << "create table hydrodynamic_configuration (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "grid_data_configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists hydrodynamic_parameter";
    sql << "create table hydrodynamic_parameter (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "value float default null, " \
        "selected bool default false, " \
        "hydrodynamic_configuration_id integer not null" \
    ")";

    sql << "drop table if exists boundary_condition";
    sql << "create table boundary_condition (" \
        "id integer primary key, " \
        "type varchar(255) not null, " \
        "object_ids varchar(255) not null, " \
        "function varchar(255) not null, " \
        "constant_value float default null, " \
        "input_module integer not null, " \
        "cell_color varchar(7) default '#FFFFFF', " \
        "vertical_integrated_outflow boolean default true, " \
        "quota float default null, " \
        "configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists time_series";
    sql << "create table time_series (" \
        "id integer primary key, " \
        "time_stamp text not null, " \
        "value float default 0, " \
        "boundary_condition_id integer not null" \
    ")";
    
    sql << "drop table if exists simulation";
    sql << "create table simulation (" \
        "id integer primary key, " \
        "label varchar(255) not null, " \
        "simulation_type integer not null, " \
        "initial_time integer not null, " \
        "simulation_period float not null, " \
        "step_time integer not null, " \
        "layers varchar(255), " \
        "observations text, " \
        "hydrodynamic_configuration_id integer" \
    ")";
    
    QSqlQuery query(currentDatabase);

    for (int i = 0; i < sql.count(); i++) {
        query.prepare(sql.at(i));

        if (!query.exec()) {
            revertConnection();
            throw DatabaseException(QString("An error occurred when creating application tables: %1").arg(query.lastError().text()));
        }
    }

    query.prepare(QString("pragma application_id = %1").arg(IPHApplication::getApplicationId()));
    query.exec();
}

bool DatabaseUtility::isDatabaseValid() {
    QSqlQuery query(currentDatabase);

    query.prepare("pragma application_id");
    query.exec();
    query.next();

    return query.value(0).toInt() == IPHApplication::getApplicationId();
}

QSqlDatabase DatabaseUtility::getDatabase() const {
    return currentDatabase;
}
