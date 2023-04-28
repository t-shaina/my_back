#ifndef DATA_BASE_H
#define DATA_BASE_H

#include<QObject>
#include <QtSql>
#include<QSqlDatabase>
#include<QTcpServer>
#include<QTcpSocket>
class Data_base_server : public QObject
{
    Q_OBJECT
public:
    explicit Data_base_server(QObject *parent = nullptr);
private:
    QTcpServer* server;
signals:

public slots:
    void newConnection();

};

#endif // DATA_BASE_H
