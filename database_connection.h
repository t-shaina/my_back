#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H

#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlDriver>

class Database_connection: public QObject
{
    Q_OBJECT

public:

    Database_connection(const QChar& request_type);
    Database_connection(const QString& request_type);

    bool open_db_connection();
    void close_db_connection();
    bool start_db_transaction();
    bool do_db_commit();
    bool do_db_rollback();
    bool has_feature(QSqlDriver::DriverFeature feature);
    QSqlDatabase get_db();

private:

    QSqlDatabase db;
};

#endif // DATABASE_CONNECTION_H

