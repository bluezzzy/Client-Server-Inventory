#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QString>
#include <QStringList>

class DBManager
{
public:
    DBManager(QString dbName);
    ~DBManager();
    void executeQuery(QString sqlQuery);
    int count(QString tableName);
    QStringList get(QString columnName, QString tableName);

private:
    QSqlDatabase db;
    
};

#endif // DBMANAGER_H
