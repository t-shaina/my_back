#include "template_query.h"
#include "database_connection.h"
#include<QSql>
#include<QSqlQuery>
#include<QList>
#include<QSqlDriver>>
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
        case select_all_id:
            query=new Select_all_query();
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
QString* Template_query::encoding_message(const QStringList* data_list){
        QString* encoded_message;
        for (int i=0;i<data_list->size();i++){
            encoded_message->push_back(static_cast<char>(data_list->at(i).size()));
            encoded_message->push_back(data_list->at(i));
        }
        return encoded_message;
}
int* Template_query::get_user_id(const QString& email){
        const QChar type_connection(6);
        Database_connection connection(type_connection);
        int* gotten_user_id=nullptr;
        if(connection.open_db_connection()){
            QSqlQuery query_get_user_id;
            query_get_user_id.prepare("SELECT user_id FROM users WHERE users_password=?");
            query_get_user_id.addBindValue(email);
            query_get_user_id.exec();
            if(query_get_user_id.next()){
                *gotten_user_id=query_get_user_id.value(0).toInt();               
            }
            connection.close_db_connection();
        }       
        return gotten_user_id;
}
QList<int>* Template_query::select_all_for_user(const int* user_id){
        QList<int>* query_result=nullptr;
        const QChar type_connection(7);
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){
            QSqlQuery query_select_all_for_user;
            query_select_all_for_user.prepare("SELECT film_id FROM films WHERE users_id=?");
            query_select_all_for_user.addBindValue(*user_id);
            query_select_all_for_user.exec();
            if(query_select_all_for_user.next()){
                query_result->push_back(query_select_all_for_user.value(0).toInt());                              
            }
            connection.close_db_connection();
        }      
        return query_result;
}
QStringList* Template_query::select_all_for_record(const int* user_id, const int* film_id, const QChar type_connection){

        QStringList* record=nullptr;
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){
            QSqlQuery query_title_year_rating_status;
            QSqlQuery query_directors;
            QSqlQuery query_genres;
            query_title_year_rating_status.prepare("SELECT films.title, films.year, films.rating, films.status"
                                                   "FROM films"
                                                   "WHERE film_id=:film_id");
            query_title_year_rating_status.bindValue(":film_id", *film_id);
            query_title_year_rating_status.exec();
            query_directors.prepare("SELECT  directors.director"
                                    "FROM films"
                                    "INNER JOIN "
                                    "(films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id)"
                                    "ON films.film_id=films_directors.film_id AND films.film_id=:film_id");
            query_directors.bindValue(":film_id", *film_id);
            query_directors.exec();
            query_genres.prepare("SELECT  genres.genre"
                                 "FROM films"
                                 "INNER JOIN "
                                 "INNER JOIN "
                                 "(films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id)"//  делать тут
                                 "ON films.film_id=films_genres.film_id AND films.film_id=:film_id");
            query_genres.bindValue(":film_id", *film_id);
            query_genres.exec();

            int director_counter=0;
            int genre_counter=0;
            QString directors;
            QString genres;
            while(query_directors.next()){
                int size=query_directors.value(director_counter).toString().size();
                directors.push_back(QString::number(size));
                directors.push_back(query_directors.value(director_counter).toString());
                director_counter++;
            }
            while(query_genres.next()){
                int size=query_genres.value(genre_counter).toString().size();
                directors.push_back(QString::number(size));
                directors.push_back(query_genres.value(genre_counter).toString());
                genre_counter++;
            }
            while(query_title_year_rating_status.next()){
                record->push_back(query_title_year_rating_status.value(0).toString());
                record->push_back(directors);
                record->push_back(genres);
                record->push_back(query_title_year_rating_status.value(1).toString());//заполнение года
                record->push_back(query_title_year_rating_status.value(2).toString());//заполнение рейтинга
                record->push_back(query_title_year_rating_status.value(3).toString());//заполнение статуса
            }
            connection.close_db_connection();
        }
        return record;
}
QStringList* Template_query::exist_query(const QStringList* decoded_message, const QChar type_connection){
        int* gotten_user_id=Template_query::get_user_id(decoded_message->at(1));
        QStringList* decoded_directors=Template_query::decoding_message(decoded_message->at(3));
        QStringList* decoded_genres=Template_query::decoding_message(decoded_message->at(4));
        QList<int>* list_of_film_id=Template_query::select_all_for_user(gotten_user_id);
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){
            for(int i=0;i<list_of_film_id->size();i++){
                for (int j=0;j<decoded_directors->size();j++){
                    QSqlQuery query;
                    query.prepare("SELECT film_id, films.title, directors.director, genres.genre, films.year, films.rating, films.status"
                                  "FROM films"
                                  "INNER JOIN "
                                  "(films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id AND directors.director=:director)"
                                  "ON films.film_id=films_directors.film_id AND films.film_id=:film_id"
                                  "WHERE films.film_id=: film_id"
                                  "AND films.title=:title"
                                  "AND films.year=:year");
                    query.bindValue(":film_id", list_of_film_id->at(i));
                    query.bindValue(":title", decoded_message->at(2));
                    query.bindValue(":director", decoded_directors->at(j));
                    query.bindValue(":year", decoded_message->at(5));
                    if (query.next()){
                        const int* gotten_film_id=&list_of_film_id->at(i);
                        QStringList* already_existing_data=Template_query::select_all_for_record(gotten_user_id, gotten_film_id);
                        connection.close_db_connection();
                        return already_existing_data;
                    }
                }
            }
        }
        return nullptr;
}
QStringList* Select_all_query::process_request(QString& message){        
        QStringList* decoded_message=Template_query::decoding_message(message);
        QStringList* query_result=nullptr;
        int* gotten_user_id=Template_query::get_user_id(decoded_message->at(1));
        QList<int>* list_of_film_id=Template_query::select_all_for_user(gotten_user_id);
            for (int i=0;i<list_of_film_id->size();i++){
                *query_result+=*Template_query::select_all_for_record(gotten_user_id, &list_of_film_id->at(i), message.at(0));
            }                      
       return query_result;
}
QStringList* Entry_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        QStringList* query_result=nullptr;
        if(connection.open_db_connection()){
            QSqlQuery query;           
            int* user_id;
            QStringList* decoded_message=Template_query::decoding_message(message);     
            query.prepare("SELECT  name, email FROM users"
                          "WHERE user_email=? "
                          "AND user_password=?");
            query.addBindValue(decoded_message->at(1));
            query.addBindValue(decoded_message->at(2));
            query.exec();
            QString name_email;
            while(query.next()){//заполнение личных данных пользователя
                int name_size=query.value(0).toString().size();
                int email_size=query.value(1).toString().size();
                name_email.push_back(QString::number(name_size));
                name_email.push_back(query.value(0).toString());
                name_email.push_back(QString::number(email_size));
                name_email.push_back(query.value(1).toString());
            }
            connection.close_db_connection();

            Template_query* query_type=Template_query::create_template_query(static_cast<Query_id>(5));
            query_result=query_type->process_request(message);// Select_all_request заполнение информации о всех фильмах для данного пользователя
            query_result->push_front(name_email);           
        }
        return query_result;
}
QStringList* Delete_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        QStringList* query_result=nullptr;
        if(connection.open_db_connection()){
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* decoded_directors=Template_query::decoding_message(decoded_message->at(2));
            QStringList* decoded_genres=Template_query::decoding_message(decoded_message->at(3));            
            int* gotten_user_id=Template_query::get_user_id(decoded_message->at(0));
            int gotten_film_id;
            for(int i=0; i<decoded_directors->size();i++){
                for(int j=0; j<decoded_genres->size();j++){
                    query.prepare("DELETE"
                              "FROM films"
                              "FULL JOIN "
                              "(films_directors FULL JOIN directors ON films_directors.director_id=directors.director_id AND directors.director=:director)"
                              "ON films.film_id=films_directors.film_id"

                              "FULL JOIN "
                              "(films_genres FULL JOIN genres ON films_genres.genre_id=genres.genre_id AND genres.genre=:genre)"
                              "ON films.film_id=films_genres.film_id"
                              "WHERE user_id=:gotten_user_id"
                              "AND films.title=:title"
                              "AND films.year=:year"
                              "AND films.rating=:rating"
                              "AND films.status=:status");
                    query.bindValue(":gotten_user_id", *gotten_user_id);
                    query.bindValue(":title", decoded_message->at(2));
                    query.bindValue(":director", decoded_directors->at(i));
                    query.bindValue(":genre", decoded_genres->at(j));
                    query.bindValue(":year", decoded_message->at(5));
                    query.bindValue(":rating", decoded_message->at(6));
                    query.bindValue(":status", decoded_message->at(7));
                    query.exec();
                }
            }
            if(query.next()){
                query_result->push_back("OK");
            }
            connection.close_db_connection();           
        }
        return query_result;
}
QStringList* Select_query::process_request(QString& message){
        Database_connection connection(message.at(0));
         QStringList* query_result=nullptr;
        if(connection.open_db_connection()){
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);           
            int* gotten_user_id=Template_query::get_user_id(decoded_message->at(1));
            QList<int>* list_of_film_id=Template_query::select_all_for_user(gotten_user_id);
            for(int i=0; i<list_of_film_id->size();i++){
                query.prepare("SELECT films.title, directors.director, genres.genre, films.year, films.rating, films.status"
                            "FROM films"
                            "INNER JOIN "
                              "(films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id)"
                            "ON films.film_id=films_directors.film_id AND films.film_id=:film_id"

                            "INNER JOIN "
                              "(films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id)"
                            "ON films.film_id=films_genres.film_id AND films.film_id=:film_id"
                            "WHERE films.title=: src_line"
                            "OR directors.director=:src_line"
                            "OR genres.genre=:src_line");
                query.bindValue(":film_id", list_of_film_id->at(i));
                query.bindValue(":src_line", decoded_message->at(2));
                query.exec();
                while(query.next()){
                    for(int j=0; j<6;j++){
                        query_result->push_back(query.value(j).toString());
                    }
                }
            }
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
QStringList* Update_query::process_request(QString& message){
        QStringList* query_result=nullptr;
        QStringList* decoded_message=Template_query::decoding_message(message);
        query_result=Template_query::exist_query(decoded_message, message.at(0));
        if (query_result==nullptr){
            Database_connection connection(message.at(0));
            if(connection.open_db_connection()){
                QSqlQuery query_films_update;
                QStringList* decoded_directors=Template_query::decoding_message(decoded_message->at(2));
                QStringList* decoded_genres=Template_query::decoding_message(decoded_message->at(3));
                int* gotten_user_id;
                int gotten_film_id;
                gotten_user_id=Template_query::get_user_id(decoded_message->at(1));

                query_films_update.prepare("INSERT INTO films (title, year, rating, status, user_id)"
                                           "VALUES (?, ?, ?, ?, ?)"
                                           "RETURNING film_id");// или film_pk
                query_films_update.addBindValue(decoded_message->at(2));
                query_films_update.addBindValue(decoded_message->at(5));
                query_films_update.addBindValue(decoded_message->at(6));
                query_films_update.addBindValue(decoded_message->at(7));
                query_films_update.addBindValue(*gotten_user_id);
                query_films_update.exec();
                if (connection.has_feature(QSqlDriver::LastInsertId)){
                    gotten_film_id=query_films_update.lastInsertId().toInt();
                }
                else if(query_films_update.next()){
                    gotten_film_id=query_films_update.value(0).toInt();

                }
                bool directors_insertion_state=Update_query::directors_update(decoded_directors, gotten_film_id, message.at(0));
                bool genres_insertion_state=Update_query::genres_update(decoded_genres, gotten_film_id, message.at(0));
                connection.close_db_connection();
            }           
        }
        //если все-таки  update
        return query_result;
}
QStringList* Registration_query::process_request(QString& message){
        Database_connection connection(message.at(0));
          QStringList* query_result=nullptr;
        if(connection.open_db_connection()){
            QSqlQuery query_existance;
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);        
            query_existance.prepare("SELECT user_name FROM users"
                                    "WHERE user_email=?"
                                    "AND password=?");
            query_existance.addBindValue(decoded_message->at(1));
            query_existance.addBindValue(decoded_message->at(2));
            query_existance.exec();
            if (query_existance.next()){
                query_result->push_back("EXIST");
            }
            else{
                query.prepare("INSERT INTO users (user_id, user_name, user_password, user_email)"
                              "VALUES (:name, :password, :email)");
                query.bindValue(":name", decoded_message->at(3));
                query.bindValue(":email", decoded_message->at(1));
                query.bindValue(":password", decoded_message->at(2));
                query.exec();
                while(query.next()){
                    query_result->push_back(decoded_message->at(1));
                    query_result->push_back(decoded_message->at(2));
                }
            }
            connection.close_db_connection();           
        }
        return query_result;
}
bool Update_query::directors_update(QStringList* directors, int gotten_film_id, const QChar type_connection){
        Database_connection connection(type_connection);
        bool update_state=false;
        if(connection.open_db_connection()){
            for(int i=0; directors->size(); i++){
                QSqlQuery query_directors_update;
                QSqlQuery query_films_directors_update;
                int gotten_director_id;
                query_directors_update.prepare("INSERT INTO directors (director)"
                                               "VALUES (?)"
                                               "RETURNING director_id"
                                               "ON CONFLICT (director) DO NOTHING");
                query_directors_update.addBindValue(directors->at(i));
                query_directors_update.exec();
                if(query_directors_update.next()){
                    gotten_director_id=query_directors_update.value(0).toInt();
                }
                else{
                    QSqlQuery query_gettin_already_exist_director_id;
                    query_gettin_already_exist_director_id.prepare("SELECT director_id FROM directors"
                                                                "WHERE director=:director");
                    query_gettin_already_exist_director_id.bindValue(":director", directors->at(i));
                    query_gettin_already_exist_director_id.exec();
                    if(query_gettin_already_exist_director_id.next()){
                        gotten_director_id=query_gettin_already_exist_director_id.value(0).toInt();
                    }
                }
                query_films_directors_update.prepare("INSERT INTO films_directors (film_id, director_id)"
                                                     "VALUES (?, ?)");
                query_films_directors_update.addBindValue(gotten_film_id);
                query_films_directors_update.addBindValue(gotten_director_id);
                query_films_directors_update.exec();
            }
            update_state=true;
            connection.close_db_connection();
        }
        return update_state;
}
bool Update_query::genres_update(QStringList* genres, int gotten_film_id, const QChar type_connection){
        Database_connection connection(type_connection);
        bool update_state=false;
        if(connection.open_db_connection()){
            for(int i=0; genres->size(); i++){
                QSqlQuery query_getting_genre_id;
                QSqlQuery query_films_genres_update;
                int gotten_genre_id;
                query_getting_genre_id.prepare("SELECT genre_id FROM genres"
                                               "WHERE genre=?");
                query_getting_genre_id.addBindValue(genres->at(i));
                query_getting_genre_id.exec();
                if(query_getting_genre_id.next()){
                    gotten_genre_id=query_getting_genre_id.value(0).toInt();
                }
                query_films_genres_update.prepare("INSERT INTO films_genres (film_id, genre_id)"
                                                     "VALUES (?, ?)");
                query_films_genres_update.addBindValue(gotten_film_id);
                query_films_genres_update.addBindValue(gotten_genre_id);
                query_films_genres_update.exec();
            }
            update_state=true;
            connection.close_db_connection();
        }
        return update_state;
}
