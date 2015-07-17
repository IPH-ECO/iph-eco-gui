#ifndef DATABASEUTILITY_H
#define DATABASEUTILITY_H

#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class DatabaseUtility {
private:
    static DatabaseUtility *instance;
    QSqlDatabase database;
    
    DatabaseUtility();
public:
    static DatabaseUtility* getInstance();
    
    void connect(const QString &databaseName);
    void disconnect();
    void createApplicationTables();
    bool isDatabaseValid();
    QSqlDatabase getDatabase() const;
};

#endif // DATABASEUTILITY_H
