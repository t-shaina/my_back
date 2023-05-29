#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H

#include <QObject>
#include<QSql>
#include<QSqlDatabase>
class Database_connection: public QObject
{
    Q_OBJECT
public:
    Database_connection(const QChar& request_type);
    bool open_db_connection();
    void close_db_connection();
    QSqlDatabase db;
};

#endif // DATABASE_CONNECTION_H
