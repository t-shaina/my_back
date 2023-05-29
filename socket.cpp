#include "socket.h"
#include "template_query.h"
#include<QString>
#include<QStringList>
#include<QChar>

Socket::Socket(QObject *parent)
    : QObject{parent}
{
    server= new QTcpServer(this);

    connect(server, SIGNAL(newConnection()),this,SLOT(newConnection()));
    if(!server->listen(QHostAddress::Any,1234)){
        qDebug() << "Server could not start";
    } else {
        qDebug() << "Server started";
    }


}

void Socket::newConnection(){
    QTcpSocket* socket=server->nextPendingConnection();
    //socket->waitForBytesWritten(1000);
    socket->waitForReadyRead(1000);
    QString message=socket->readAll();

    Query_id query_id=static_cast<Query_id>(message.at(0).digitValue());
    Template_query* query_type=Template_query::create_template_query(query_id);

    QStringList* result=query_type->process_request(message);

}
