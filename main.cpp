
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
        QList<int>* films_id=new QList<int>();
        QSqlQuery query_select_all_for_user(db.get_db());
        query_select_all_for_user.prepare("SELECT film_id FROM films "
                                          "WHERE films.user_id=(SELECT users.user_id "
                                                                "FROM users "
                                                                "WHERE users.user_email=:email) ");
        query_select_all_for_user.bindValue(":email", "tanya1@mail.ru");
        qDebug()<<"last error after prepare in query select all for user"<<query_select_all_for_user.lastError();
        query_select_all_for_user.exec();
        qDebug()<<"last error in query select all"<<query_select_all_for_user.lastError();
        while (query_select_all_for_user.next()){
            films_id->push_back(query_select_all_for_user.value(0).toInt());
            qDebug()<<"film id  is  "<<query_select_all_for_user.value(0).toString();
        }


        for(int i=0; i<films_id->size();++i){
            QSqlQuery query(db.get_db());
            query.prepare("SELECT films.title, directors.director, genres.genre, films.year, films.rating, films.status "
                          "FROM films "

                          "INNER JOIN "
                          "films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id "
                          "ON films.film_id=films_directors.film_id AND films.film_id=:film_id "

                          "INNER JOIN "
                          "films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id "
                          "ON films.film_id=films_genres.film_id AND films.film_id=:film_id "
                          "WHERE "
                          "films.title=:src_line "
                          "OR directors.director=:src_line "
                          "OR genres.genre=:src_line");
            query.bindValue(":email", "tanya1@mail.ru");
            query.bindValue(":src_line", "Король Лев");
            query.bindValue(":film_id", films_id->at(i));
            qDebug()<<"last error after prepare"<<query.lastError();
            query.exec();
            qDebug()<<"exec is"<<query.exec();
            qDebug()<<"last error"<<query.lastError();
            int j=0;
            if (query.next()){

                qDebug()<<"Title is  "<<query.value(0).toString();

                j++;
            }
        }

    }
    else{

        qDebug() << "Error = " << db.get_db().lastError().text();
    }*/

    return a.exec();
}
