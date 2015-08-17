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
    QSqlDatabase currentDatabase;
    QSqlDatabase *previousDatabase;
    
    DatabaseUtility();
public:
    static DatabaseUtility* getInstance();
    
    void connect(const QString &databaseName, bool force = false);
    void disconnect();
    void revertConnection();
    void createApplicationTables();
    bool isDatabaseValid();
    QSqlDatabase getDatabase() const;
};

#endif // DATABASEUTILITY_H
