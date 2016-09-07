#include <utility/database_utility.h>

#include <application/iph_application.h>
#include <exceptions/database_exception.h>

#include <QUuid>
#include <QFile>
#include <QList>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

DatabaseUtility* DatabaseUtility::instance = nullptr;

DatabaseUtility::DatabaseUtility() : previousDatabase(nullptr) {}

DatabaseUtility* DatabaseUtility::getInstance() {
    if (!instance) {
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
            throw DatabaseException(QString("The following error ocurred during database connection: %1.").arg(currentDatabase.lastError().text()).toStdString());
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
        "water_quality boolean default false" \
    ")";

    sql << "drop table if exists mesh";
    sql << "create table mesh (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "coordinates_distance float default 0, " \
        "mesh_poly_data text not null, " \
        "boundary_poly_data text not null, " \
        "resolution float" \
    ")";

    sql << "drop table if exists mesh_polygon";
    sql << "create table mesh_polygon (" \
        "id integer primary key, " \
        "name varchar(255) not null, " \
        "type varchar(255) not null, " \
        "original_poly_data text not null, " \
		"filtered_poly_data text not null, " \
		"minimum_angle float, " \
        "maximum_edge_length float, " \
        "latitude_average float default 0, " \
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
        "enabled bool default true, " \
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
        "minimum_elevation float default null, " \
        "maximum_elevation float default null, " \
        "configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists time_series";
    sql << "create table time_series (" \
        "time_stamp integer not null, " \
        "value1 float default 0, " \
        "value2 float default 0, " \
        "object_id integer not null, " \
        "object_type text not null" \
    ")";
    
    sql << "drop table if exists water_quality_configuration";
    sql << "create table water_quality_configuration (" \
        "id integer primary key, " \
        "name text not null, " \
        "grid_data_configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists water_quality_parameter";
    sql << "create table water_quality_parameter (" \
        "id integer primary key, " \
        "name text not null, " \
        "section integer not null, " \
        "value float default 0, " \
        "group_values text default null, " \
        "water_quality_configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists food_matrix";
    sql << "create table food_matrix (" \
        "predator text not null, " \
        "prey text not null, " \
        "value double default 0, " \
        "water_quality_configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists meteorological_configuration";
    sql << "create table meteorological_configuration (" \
        "id integer primary key, " \
        "name text not null, " \
        "grid_data_configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists meteorological_station";
    sql << "create table meteorological_station (" \
        "id integer primary key, " \
        "name text not null, " \
        "use_latitude_longitude bool default false, " \
        "utm_x float not null, " \
        "utm_y float not null, " \
        "latitude float default 0, " \
        "longitude float default 0, " \
        "meteorological_configuration_id integer not null" \
    ")";
    
    sql << "drop table if exists meteorological_parameter";
    sql << "create table meteorological_parameter (" \
        "id integer primary key, " \
        "name text not null, " \
        "unit text, " \
        "function integer not null, " \
        "constant_value float, " \
        "use_xy_component bool default false, " \
        "x_component float default 0, " \
        "y_component float default 0, " \
        "intensity float default 0, " \
        "direction float default 0, " \
        "meteorological_station_id integer not null" \
    ")";
    
    sql << "drop table if exists simulation";
    sql << "create table simulation (" \
        "id integer primary key, " \
        "label varchar(255) not null, " \
        "simulation_type integer not null, " \
        "start_time integer not null, " \
        "initial_time integer not null, " \
        "period float not null, " \
        "step_time integer not null, " \
        "minimum_vertical_limit float, " \
        "maximum_vertical_limit float, " \
        "layers varchar(255), " \
        "observations text, " \
        "output_time_interval integer, " \
        "output_directory text, " \
        "autosave_time_interval integer, " \
        "output_parameters text, " \
        "status integer default 1, " \
        "progress integer default 0, " \
        "recovery_variables text, " \
        "hydrodynamic_configuration_id integer, " \
        "meteorological_configuration_id integer" \
    ")";
    
    QSqlQuery query(currentDatabase);

    for (int i = 0; i < sql.count(); i++) {
        query.prepare(sql.at(i));

        if (!query.exec()) {
            revertConnection();
            throw DatabaseException(QString("An error occurred when creating application tables: %1").arg(query.lastError().text()).toStdString());
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
