
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
    /*Database_connection db("9");

    if(db.open_db_connection()){

        qDebug()<<"in main Opened";
        QSqlQuery query(db.get_db());
        QStringList genres;
        QString genre="Драма";
        genres.push_back(genre);
        qDebug()<<"genre before prepare is"<<genre;
        query.prepare( "SELECT films.film_id "
                                  "FROM "
                                  "films INNER JOIN "
                                  "films_directors  INNER JOIN "
                                  "directors ON films_directors.director_id=directors.director_id  AND directors.director=:director "
                                  "ON films.film_id=films_directors.film_id "

                                  "WHERE films.user_id=(SELECT users.user_id "
                                                        "FROM users "
                                                        "WHERE users.user_email=:email)"
                                  "AND films.title=:title AND films.year=:year ");
                query.bindValue(":email", "tanya1@mail.ru");

                query.bindValue(":genre", genre);
                query.bindValue(":title", "Зеленая миля");
                query.bindValue(":year", "1999");
                query.bindValue(":rating", "1");
                query.bindValue(":status", "Status");
                query.bindValue(":director", "Дарабонт");

        qDebug()<<"last error after prepare"<<query.lastError();
        int i=0;
        //query.exec();
        //qDebug()<<"is active"<<query_existance.isActive();
        qDebug()<<"exec is"<<query.exec();
        qDebug()<<"last error"<<query.lastError();
        if (query.next()){

            qDebug()<<"ROW IS EXIST "<<query.value(0).toString();
            qDebug()<<"last error"<<query.lastError();
            i++;
        }
    }
    else{

        qDebug() << "Error = " << db.get_db().lastError().text();
    }*/

    return a.exec();
}
