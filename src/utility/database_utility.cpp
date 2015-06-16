#include "include/utility/database_utility.h"

#include <QUuid>

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

    //TODO: Finish tables creation

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
           "grid_data_configuration_id integer not null, " \
           "mesh_id integer not null" \
    ")";

    QSqlQuery query(database);

    for (int i = 0; i < sql.count(); i++) {
        query.prepare(sql.at(i));

        if (!query.exec()) {
            throw DatabaseException(QString("The following error ocurred during database creation: %1").arg(query.lastError().text()));
        }
    }

    query.prepare(QString("pragma application_id = %1").arg(IPHApplication::getApplicationId()));
    query.exec();
}

bool DatabaseUtility::isDatabaseValid(QSqlDatabase &database) {
    QSqlQuery query(database);

    query.prepare("pragma application_id");
    query.exec();
    query.next();

    return query.value(0).toInt() == IPHApplication::getApplicationId();
}
