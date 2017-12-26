#include "dbmanager.h"

DBManager::DBManager(QString dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localHost");
    db.setDatabaseName(dbName);

    db.open();

}

DBManager::~DBManager()
{
    db.close();
}

void DBManager::executeQuery(QString sqlQuery)
{
    QSqlQuery query;
    query.exec(sqlQuery);
}

int DBManager::count(QString tableName)
{
    QSqlQuery query;
    query.exec(QString("SELECT COUNT(*) FROM %1").arg(tableName));
    query.next();
    return query.value(0).toInt();
}

QStringList DBManager::get(QString columnName, QString tableName)
{
    QSqlQuery query;
    query.exec(QString("SELECT %1 FROM %2").arg(columnName).arg(tableName));
    QStringList records;
    while (query.next())
    {
        records.append(query.value(0).toString());
    }
    return records;
}