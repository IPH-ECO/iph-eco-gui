#ifndef DATABASEUTILITY_H
#define DATABASEUTILITY_H

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "include/exceptions/databaseexception.h"

class DatabaseUtility {
public:
    static QSqlDatabase connect(QString &database_name);
    static void disconnect(QSqlDatabase &database);
    static void createApplicationDatabase(QSqlDatabase &database);
};

#endif // DATABASEUTILITY_H
