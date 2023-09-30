
#include "database_connection.h"
#include <QCoreApplication>
#include "test_server.h"
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
#include<QList>
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
    qDebug()<<"transaction"<<db.has_feature(QSqlDriver::Transactions);
    if(db.open_db_connection()){
        QList<QString>* films_id=new QList<QString>();
        QSqlQuery query(db.get_db());
        query.prepare("WITH current_user_id AS(SELECT users.user_id "
                      "FROM users "
                      "WHERE users.user_email=:email "
                      "LIMIT 1), "
                      "current_films_id AS(SELECT films.film_id "
                      "FROM films "
                      "WHERE films.user_id IN(SELECT user_id FROM current_user_id) "
                      "AND films.title=:title AND films.year=:year AND films.rating=:rating AND films.status=:status), "
                      "current_director AS(SELECT directors.director "
                      "FROM directors INNER JOIN films_directors ON directors.director_id=films_directors.director_id "
                      "WHERE directors.director=:director "
                      "AND films_directors.film_id IN (SELECT film_id FROM current_films_id) "
                      "ORDER BY director "
                      "LIMIT 1), "
                      "current_film_id AS(SELECT films.film_id "
                      "FROM films INNER JOIN films_directors "
                      "INNER JOIN directors ON directors.director_id=films_directors.director_id "
                      "AND directors.director IN (SELECT director FROM current_director) "
                      "ON films.film_id=films_directors.film_id "
                      "AND films.film_id IN(SELECT film_id FROM current_films_id)) "
                      "DELETE FROM films WHERE films.film_id IN (SELECT film_id FROM current_film_id) "
                      "RETURNING films.film_id");
        query.bindValue(":email", "tanya1@mail.ru");
        query.bindValue(":title", "test1");
        query.bindValue(":year", "1994");
        query.bindValue(":rating", "5");
        query.bindValue(":status", "Просмотрено");
        query.bindValue(":director", "Дарабонт");
        qDebug()<<"last error in query "<<query.lastError();
        query.exec();
        qDebug()<<"last error in query select all"<<query.lastError();
        while (query.next()){
            films_id->push_back(query.value(1).toString());
            qDebug()<<"film id  is  "<<query.value(0).toString();
        }


        for(int i=0; i<films_id->size();++i){

            qDebug()<<"title is is"<<films_id->at(i);
        }

    }
    else{

        qDebug() << "Error = " << db.get_db().lastError().text();
    }*/

    return a.exec();
}
