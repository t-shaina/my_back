
#include "database_connection.h"
#include <QCoreApplication>
#include "test_server.h"
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Test_server* server=new Test_server(&a);

    /*QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setPort(5432);
    db.setDatabaseName("films_info");
    db.setUserName("tanya");
    db.setPassword("123456");*/
    Database_connection db("9");
    qDebug()<<"have prepared"<<db.has_feature(QSqlDriver::PreparedQueries);
    qDebug()<<"have named placeholders"<<db.has_feature(QSqlDriver::NamedPlaceholders);
    qDebug()<<"have positional  placeholders"<<db.has_feature(QSqlDriver::PositionalPlaceholders);
    qDebug()<<"have last insert id"<<db.has_feature(QSqlDriver::LastInsertId);
    if(db.open_db_connection()){

        qDebug()<<"in main Opened";
        QSqlQuery query_existance(db.get_db());
        query_existance.prepare("INSERT INTO users (user_id, user_name, user_password, user_email)"
                                "VALUES (DEFAULT, :name, :password, :email)"
                                "RETURNING user_id");
        query_existance.bindValue(":name", "name");
        query_existance.bindValue(":email", "email");
        query_existance.bindValue(":password", "123456");
        qDebug()<<"last error after prepare"<<query_existance.lastError();
        int i=0;
        query_existance.exec();
        //qDebug()<<"is active"<<query_existance.isActive();
        qDebug()<<"is valid"<<query_existance.isValid();
        while (query_existance.next()){

            qDebug()<<query_existance.value(0).toString();
            //qDebug()<<"last error"<<query_existance.lastError();
            i++;
        }
    }
    else{

        qDebug() << "Error = " << db.get_db().lastError().text();
    }

    return a.exec();
}
