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

void DatabaseUtility::createApplicationDatabase(QSqlDatabase &database) {
    QStringList sql;

    //TODO: Finish tables creation

    sql << "drop table if exists project";
    sql << "create table project (" \
       "id integer primary key, " \
       "name varchar(255) not null, " \
       "description text, " \
       "hydrodynamic boolean," \
       "water_quality boolean," \
       "sediment boolean" \
    ")";

    sql << "drop table if exists structured_mesh";
    sql << "create table structured_mesh (" \
           "id integer primary key, " \
           "name varchar(255) not null, " \
           "resolution integer not null, " \
           "data text not null, " \
           "project_id integer" \
    ")";

    sql << "drop table if exists islands";
    sql << "create table islands (" \
           "id integer primary key, " \
           "mesh_type varchar(255) not null, " \
           "coordinates_json text not null" \
    ")";

    sql << "drop table if exists unstructured_mesh";
    sql << "create table unstructured_mesh (" \
           "id integer primary key, " \
           "data text not null, " \
           "radius float not null, " \
           "project_id integer" \
    ")";

    QSqlDatabase::database().transaction();
    QSqlQuery query(database);

    for (int i = 0; i < sql.count(); i++) {
        query.prepare(sql.at(i));

        if (!query.exec()) {
            QSqlDatabase::database().rollback();
            DatabaseUtility::disconnect(database);

            throw DatabaseException(QString("The following error ocurred during database creation: %1").arg(query.lastError().text()));
        }
    }

    query.prepare(QString("pragma application_id = %1").arg(IPHApplication::getApplicationId()));
    query.exec();

    QSqlDatabase::database().commit();
}

bool DatabaseUtility::isDatabaseValid(QSqlDatabase &database) {
    QSqlQuery query(database);

    query.prepare("pragma application_id");
    query.exec();
    query.next();

    return query.value(0).toInt() == IPHApplication::getApplicationId();
}
