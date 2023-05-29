#ifndef SOCKET_H
#define SOCKET_H

#include<QObject>

#include<QSqlDatabase>
#include<QTcpServer>
#include<QTcpSocket>
class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QObject *parent = nullptr);
private:
    QTcpServer* server;
signals:

public slots:
    void newConnection();

};

#endif // SOCKET_H
