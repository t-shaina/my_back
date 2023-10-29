#include "database_connection.h"
#include "qdebug.h"
#include "qsqlerror.h"

Database_connection::Database_connection(const QChar& request_type)
{
    QString connection_name= QString ("connection_");
    connection_name+= request_type;
    db=QSqlDatabase::addDatabase("QPSQL", connection_name);
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("films_info");
    db.setUserName("tanya");
    db.setPassword("123456");

} // на отступы между определениями функций бюджета не хватило? =*)
Database_connection::Database_connection(const QString& request_type){
    QString connection_name= QString ("connection_");
    connection_name+= request_type;
    qDebug()<<"db_connection name is: "<<connection_name;
    db=QSqlDatabase::addDatabase("QPSQL", connection_name);
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("films_info");
    db.setUserName("tanya");
    db.setPassword("123456");
}
bool Database_connection::open_db_connection(){
    if(db.open()){
        qDebug()<<"in class Opened";
        return true;        
    }
    else{
        qDebug() << "Error = " << db.lastError().text();
        return false;       
    }
}
void Database_connection::close_db_connection(){
    qDebug()<<"last error in clode-db_connectiom is"<<db.lastError();
    db.close();
}
bool Database_connection::start_db_transaction(){
    if(db.transaction()){
        return true;
    }
    else return false;
}
bool Database_connection::do_db_commit(){
    return db.commit();
}
bool Database_connection::do_db_rollback(){
    return db.rollback();
}
bool Database_connection::has_feature(QSqlDriver::DriverFeature feature){
    if(db.driver()->hasFeature(feature)) return true;
    else return false;
}
QSqlDatabase Database_connection::get_db(){
    return this->db;
}
