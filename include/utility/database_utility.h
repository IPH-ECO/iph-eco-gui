#ifndef DATABASEUTILITY_H
#define DATABASEUTILITY_H

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "include/application/iph_application.h"
#include "include/exceptions/database_exception.h"

class DatabaseUtility {
public:
    static QSqlDatabase connect(QString &database_name);
    static void disconnect(QSqlDatabase &database);
    static void createApplicationTables(QSqlDatabase &database);
    static bool isDatabaseValid(QSqlDatabase &database);
};

#endif // DATABASEUTILITY_H
