#include "database_connection.h"
#include "qdebug.h"
#include "qsqlerror.h"

Database_connection::Database_connection(const QChar& request_type)
{
    QString connection_name= QString ("connection_");
    connection_name+= request_type;
    db=QSqlDatabase::addDatabase("QPSQL", connection_name);//тут другое имя соединения
    db.setHostName("my_host");
    db.setDatabaseName("films_info");
    db.setUserName("Tanya");
    db.setPassword("123456");

}
bool Database_connection::open_db_connection(){
    if(db.open()){
        return true;
        qDebug()<<"Opened";
    }
    else{
        return false;
        qDebug() << "Error = " << db.lastError().text();
    }
}
void Database_connection::close_db_connection(){
    db.close();
}
