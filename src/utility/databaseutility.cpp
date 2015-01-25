#include "include/utility/databaseutility.h"

QSqlDatabase DatabaseUtility::connect(QString &database_name) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(database_name);

    if (!db.open()) {
        QString error = QString("The following error ocurred during database connection: %1").arg(db.lastError().text());
        throw DatabaseException(error.toStdString().c_str());
    }

    return db;
}

void DatabaseUtility::disconnect(QSqlDatabase &database) {
    if (database.isOpen()) {
        database.close();
    }
}

void DatabaseUtility::createApplicationDatabase(QSqlDatabase &database) {
    QStringList sql;

    //TODO: Finish tables creation

    sql << "drop table if exists project";
    sql << "create table project (" \
       "id integer primary key, " \
       "name varchar(255) not null, " \
       "description text, " \
       "analysis varchar(255) not null," \
       "selected_grid_data_id integer," \
       "selected_hydrodynamic_data_id integer," \
       "version integer" \
    ")";

    sql << "drop table if exists structured_grid";
    sql << "create table structured_grid (" \
       "id integer primary key, " \
       "data text not null, " \
       "project_id integer" \
    ")";

    sql << "drop table if exists unstructured_grid";
    sql << "create table unstructured_grid (" \
       "id integer primary key, " \
       "data text not null, " \
       "radius float not null, " \
       "project_id integer" \
    ")";

    QSqlDatabase::database().transaction();

    for (int i = 0; i < sql.count(); i++) {
        QSqlQuery query(database);

        query.prepare(sql.at(i));

        if (!query.exec()) {
            QSqlDatabase::database().rollback();
            DatabaseUtility::disconnect(database);

            QString error = QString("The following error ocurred during database creation: %1").arg(query.lastError().text());
            throw DatabaseException(error.toStdString().c_str());
        }
    }

    QSqlDatabase::database().commit();
}
