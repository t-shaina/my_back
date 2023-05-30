#include "template_query.h"
#include "database_connection.h"
#include<QSql>
#include<QSqlQuery>
Template_query::Template_query()
{}

Template_query::~Template_query()
{}
Template_query* Template_query::create_template_query(Query_id id){
    Template_query* query;
    switch(id){
        case entry_id:
            query=new Entry_query();
            break;
        case delete_id:
            query=new Delete_query();
            break;
        case select_id:
            query=new Select_query();
            break;
        case update_id:
            query=new Update_query();
            break;
        case registration_id:
            query=new Registration_query();
            break;
        default:
            query=nullptr;
            break;
        }
        return query;
}
QStringList* Template_query::decoding_message(const QString& message){
        QStringList* decoded_message;
        for (int i=1; i<message.size();i++){
            int length=message.at(i).digitValue();
            decoded_message->push_back(message.mid(i+1, i+length));
        }
        return decoded_message;
}
int* Template_query::get_user_id(const QString& email){
        const QChar type_connection(5);
        Database_connection connection(type_connection);
        int* gotten_user_id;
        if(connection.open_db_connection()){
            QSqlQuery query_get_user_id;
            query_get_user_id.prepare("SELECT user_id FROM users WHERE users_password=?");
            query_get_user_id.addBindValue(email);
            query_get_user_id.exec();
            if(query_get_user_id.next()){
                *gotten_user_id=query_get_user_id.value(0).toInt();
                return gotten_user_id;
            }
            else return nullptr;
        }
        connection.close_db_connection();
        return nullptr;
}
QStringList* Entry_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        if(connection.open_db_connection()){
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* query_result;
            query.prepare("SELECT user_name FROM users"
                       "WHERE user_email=? "
                       "AND user_password=?");
            query.addBindValue(decoded_message->at(0));
            query.addBindValue(decoded_message->at(1));
            query.exec();
            while(query.next()){
                query_result->push_back(query.value(0).toString());
            }
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
QStringList* Delete_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        if(connection.open_db_connection()){
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* query_result;
            query.prepare("DELETE FROM films WHERE = decoded_message.at(0) AND password=decoded-message.at(1)");///делать тут
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
QStringList* Select_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        if(connection.open_db_connection()){
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* query_result;
            int* gotten_user_id=Template_query::get_user_id(decoded_message->at(0));
            query.prepare("SELECT films.title, directors.director, genres.genre, films.year, films.rating, films.status"
                          "FROM films, directors, genres"
                          "WHERE films.user_id=:id"
                          "AND (films.title=src_line"
                          "OR directors.director=(SELECT director FROM directors "//делать тут
                       );
            query.bindValue(":id", *gotten_user_id);
            query.bindValue(":src_line", decoded_message->at(1));
            query.exec();
            while(query.next()){
                query_result->push_back(query.value(0).toString());
            }
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
QStringList* Update_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        if(connection.open_db_connection()){
            QSqlQuery query_existance;
            QSqlQuery query_films_update;
            QSqlQuery query_directors_update;
            QSqlQuery query_films_directors_update;
            QSqlQuery query_genres_update;
            QSqlQuery query_films_genres_update;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* decoded_directors=Template_query::decoding_message(decoded_message->at(2));
            QStringList* decoded_genres=Template_query::decoding_message(decoded_message->at(3));
            QStringList* query_result;
            int* gotten_user_id;
            int gotten_film_id;

            gotten_user_id=Template_query::get_user_id(decoded_message->at(0));
            query_existance.exec("SELECT user_name FROM users"
                                 "WHERE user_email= decoded_message.at(1)"
                                 "AND password=decoded-message.at(2)");
            if (query_existance.next()){
                query_result->push_back("0");
            }
            else{
                query_films_update.prepare("INSERT INTO films (film_id, title, year, rating, status, user_id)"
                                           "VALUES (?, ?, ?, ?, ?)");
                query_films_update.addBindValue(decoded_message->at(1));
                query_films_update.addBindValue(decoded_message->at(4));
                query_films_update.addBindValue(decoded_message->at(5));
                query_films_update.addBindValue(decoded_message->at(6));
                query_films_update.addBindValue(*gotten_user_id);
                query_films_update.exec();
                if (query_films_update.next()){
                    gotten_film_id=query_films_update.value(0).toInt();
                }
                for(int i=0; i<decoded_directors->size(); i++){
                    query_directors_update.prepare("INSERT INTO directors (director_id, director)"
                                                "VALUES (?))"
                                                "ON CONFLICT (director) DO NOTHING");
                    query_directors_update.addBindValue(decoded_directors->at(i));
                    query_directors_update.exec();
                    query_films_directors_update.prepare("INSERT INTO films_directors (film_id, director_id, film_director_id)"
                                                      "SELECT films.film_id, directors.director_id"
                                                      "FROM films, directors"
                                                      "WHERE film_id=?"
                                                      "AND director_id=(SELECT director_id FROM directors WHERE director=?)");
                    query_films_directors_update.addBindValue(gotten_film_id);
                    query_films_directors_update.addBindValue(decoded_directors->at(i));
                    query_films_directors_update.exec();
                }
                for(int i=0; i<decoded_genres->size(); i++){
                    /*query_genres_update.prepare("INSERT INTO genres (genre_id, genre)"
                                             "VALUES (?)"
                                             "ON CONFLICT (genre) DO NOTHING");
                    query_genres_update.addBindValue(decoded_genres->at(i));
                        query_genres_update.exec();*/
                    query_films_genres_update.prepare("INSERT INTO films_genres (film_id, genre_id, film_genre_id)"
                                                         "SELECT films.film_id, genres.genre_id"
                                                         "FROM films, genres"
                                                         "WHERE film_id=?"
                                                         "AND genre_id=(SELECT genre_id FROM genres WHERE genre=?)");
                    query_films_directors_update.addBindValue(gotten_film_id);
                    query_films_directors_update.addBindValue(decoded_genres->at(i));
                    query_films_directors_update.exec();
                }
            }
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
QStringList* Registration_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        if(connection.open_db_connection()){
            QSqlQuery query_existance;
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* query_result;
            query_existance.prepare("SELECT user_name FROM users"
                                 "WHERE user_email=?"
                                 "AND password=?");
            query_existance.addBindValue(decoded_message->at(1));
            query_existance.addBindValue(decoded_message->at(2));
            query_existance.exec();
            if (query_existance.next()){
                query_result->push_back("0");
            }
            else{
                query.prepare("INSERT INTO users (user_id, user_name, user_password, user_email)"
                           "VALUES (?, ?, ?)");
                query.addBindValue(decoded_message->at(0));
                query.addBindValue(decoded_message->at(2));
                query.addBindValue(decoded_message->at(1));
                query.exec();
                while(query.next()){
                    query_result->push_back(query.value(0).toString());
                }
            }
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
