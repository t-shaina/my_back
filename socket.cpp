#include "socket.h"
#include "template_query.h"
#include<QString>
#include<QStringList>
#include<QChar>

Socket::Socket(QObject *parent)
    : QObject{parent}
{
    server= new QTcpServer(this);

    // также стоит ставить пробелы после запятых - правило хорошего тона
    // напр "some_foo(param1, param2, param3);"
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
    QStringList* list_result=new QStringList();
    query_type->process_request(list_result, message);
    // пробелы между операторами(арифметическими и присваивания) и данными
    // напр "some a = b + c;"
    QString result=QString(message.at(0)); 
    result+=Template_query::encoding_message( list_result);
    int bytes=socket->write(result.toUtf8());//тут проверка

}
