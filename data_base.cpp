#include "data_base.h"
#include<QString>
#include<QStringList>

Data_base_server::Data_base_server(QObject *parent)
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

void Data_base_server::newConnection(){
    QTcpSocket* socket=server->nextPendingConnection();
    //socket->waitForBytesWritten(1000);
    socket->waitForReadyRead(1000);
    QString message=socket->readAll();

    QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL", "films_db");//тут другое имя соединения
    db.setHostName("my_host");
    db.setDatabaseName("films_db");
    db.setUserName("Tanya");
    db.setPassword("password");

    QSqlQuery query;
    QStringList results;

    if(db.open()){
        qDebug()<<"Opened";

    }
    else{
         qDebug() << "Error = " << db.lastError().text();
    }

    db.close();

}
