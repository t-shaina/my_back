#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <QObject>
#include<QTcpServer>
#include<QTcpSocket>
#include<QList>
#include<QStringList>
#include"socket_adapter.h"
#include<QJsonParseError>
#include<QJsonObject>
class Test_server:public QObject
{
    Q_OBJECT
protected:
    QTcpServer* server_;
    QList<ISocket_adapter*>* m_clients;
    bool verification(QString string);

public:
    Test_server(QObject *parent = nullptr);
    ~Test_server();
public slots:
    void on_newConnection();
    void on_disconnected();
    void on_message(QByteArray);
};


class Server_socket_adapter: public Socket_adapter
{
public:
    explicit Server_socket_adapter(QTcpSocket* socket, QObject* parent=0);

};
#endif // TEST_SERVER_H
