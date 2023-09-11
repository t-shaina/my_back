#include "template_query.h"
#include "database_connection.h"
#include<QSql>
#include<QSqlQuery>
#include<QList>
#include<QSqlDriver>
#include<QSqlError>
#include"json_creator.h"
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
        case insert_id:
            query=new Insert_query();
            break;
        case registration_id:
            query=new Registration_query();
            break;
        case select_all_id:
            query=new Select_all_query();
            break;
        case update_id:
            query=new Update_query();
            break;
        default:
            query=nullptr;
            break;
        }
        return query;
}
QStringList Template_query::decoding_json_object(const QJsonValue& object){
        QStringList decoded_object;
        QJsonArray object_array=object.toArray();

        for (int i=0; i<object_array.size();i++){//мб тут с условием выхода из for
            decoded_object.push_back(object_array.at(i).toString());
        }
        return decoded_object;
}
/*QString Template_query::encoding_message(const QStringList* data_list){
        QString encoded_message;
        for (int i=0;i<data_list->size();i++){
            QString element=data_list->at(i);
            int size=element.size();
            int size_length=Template_query::number_of_tens(size);
            encoded_message+=QString::number(size_length);
            encoded_message+=QString::number(size);
            encoded_message+=element;
        }
        return encoded_message;
}
int Template_query::number_of_tens(int size){
        int number_of_tens=1;
        while(size>9){
            size=size%10;
            number_of_tens++;
        }
        return number_of_tens;
}*/
int Template_query::get_user_id(const QString& email, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('0');
        Database_connection connection(type_connection);
        int gotten_user_id=-1;
        if(connection.open_db_connection()){
            QSqlQuery query_get_user_id(connection.get_db());
            query_get_user_id.prepare("SELECT user_id FROM users WHERE user_email=?");
            query_get_user_id.addBindValue(email);
            query_get_user_id.exec();
            if(query_get_user_id.next()){
                gotten_user_id=query_get_user_id.value(0).toInt();
            }
            connection.close_db_connection();
        }       
        return gotten_user_id;
}

int Template_query::get_film_id(const int* user_id, const QJsonObject &object, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('0');
        Database_connection connection(type_connection);
        int gotten_film_id=-1;
        if(connection.open_db_connection()){
            QVariantMap object_map=object.toVariantMap();
            QJsonObject row;
            if(object_map.contains("Row")){
                row=object_map.value("Row").toJsonObject();
            }
            else if(object_map.contains("Row_old")){
                row=object_map.value("Row").toJsonObject();
            }
            else return gotten_film_id;

            QStringList decoded_directors=Template_query::decoding_json_object(row.value("Directors"));
            QStringList decoded_genres=Template_query::decoding_json_object(row.value("Genres"));
            for(int i=0; i<decoded_directors.size();i++){
                for(int j=0; j<decoded_genres.size();j++){
                    QSqlQuery query_get_film_id(connection.get_db());
                    query_get_film_id.prepare("SELECT film_id FROM films"
                                              "FULL JOIN"
                                              "(films_directors FULL JOIN directors ON films_directors.director_id=directors.director_id AND directors.director=:director)"
                                              "ON films.film_id=films_directors.film_id"

                                              "FULL JOIN "
                                              "(films_genres FULL JOIN genres ON films_genres.genre_id=genres.genre_id AND genres.genre=:genre)"
                                              "ON films.film_id=films_genres.film_id"
                                              "WHERE films.title=:title AND films.year=:year AND films.rating=:rating, AND films.status=:status AND user_id=user_id");
                    query_get_film_id.bindValue(":title", object.value("Title").toString());
                    query_get_film_id.bindValue(":year", object.value("Year").toString());
                    query_get_film_id.bindValue(":director", decoded_directors.at(i));
                    query_get_film_id.bindValue(":genre", decoded_genres.at(j));
                    query_get_film_id.bindValue(":rating", object.value("Rating").toString());
                    query_get_film_id.bindValue(":status", object.value("Status").toString());
                    query_get_film_id.bindValue(":user_id", *user_id);
                    query_get_film_id.exec();

                    if(query_get_film_id.next()){
                        gotten_film_id=query_get_film_id.value(0).toInt();
                    }
                }
            }
            connection.close_db_connection();
        }
        return gotten_film_id;
}
void Template_query::select_all_for_user(QList<int>* films_id, const int* user_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('2');
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){
            QSqlQuery query_select_all_for_user(connection.get_db());
            query_select_all_for_user.prepare("SELECT film_id FROM films WHERE users_id=?");
            query_select_all_for_user.addBindValue(*user_id);
            query_select_all_for_user.exec();
            if(query_select_all_for_user.next()){
                films_id->push_back(query_select_all_for_user.value(0).toInt());
            }
            connection.close_db_connection();
        }      
}
QJsonObject Template_query::select_all_for_record(const int* user_id, const int* film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('3');
        Database_connection connection(type_connection);
        QJsonObject record;
        if(connection.open_db_connection()){
            QSqlQuery query_title_year_rating_status(connection.get_db());
            QSqlQuery query_directors(connection.get_db());
            QSqlQuery query_genres(connection.get_db());
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

            //int director_counter=0;
            ///int genre_counter=0;
            QJsonArray directors;
            QJsonArray genres;

            while(query_directors.next()){
                directors.push_back(query_directors.value(0).toString());//counter=0
                //director_counter++;
            }
            while(query_genres.next()){
                directors.push_back(query_genres.value(0).toString());//counter=0
                //genre_counter++;
            }
            while(query_title_year_rating_status.next()){
                record["Title"]=query_title_year_rating_status.value(0).toString();
                record["Directors"]=directors;
                record["Genres"]=genres;
                record["Year"]=query_title_year_rating_status.value(1).toString();//заполнение года
                record["Rating"]=query_title_year_rating_status.value(2).toString();//заполнение рейтинга
                record["Status"]=query_title_year_rating_status.value(3).toString();//заполнение статуса
            }
            connection.close_db_connection();
        }
        return record;

}
 bool Template_query::exist_query(const QJsonObject& object, const QString parent_type_connection){//разобрать эту
        bool exist_state=false;
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        //QStringList decoded_message=Template_query::decoding_message(message);
        int gotten_user_id=Template_query::get_user_id(email, request_code);
        QStringList decoded_directors=Template_query::decoding_json_object(object.value("Directors"));
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('4');
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){
            QList<int>* list_of_film_id=new QList<int>();
            Template_query::select_all_for_user(list_of_film_id, &gotten_user_id, request_code);
            for(int i=0;i<list_of_film_id->size();i++){
                for (int j=0;j<decoded_directors.size();j++){
                    QSqlQuery query(connection.get_db());
                    query.prepare("SELECT film_id, films.title, directors.director, genres.genre, films.year, films.rating, films.status"
                                  "FROM films"
                                  "INNER JOIN "
                                  "(films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id AND directors.director=:director)"
                                  "ON films.film_id=films_directors.film_id AND films.film_id=:film_id"
                                  "INNER JOIN "
                                  "(films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id)"
                                  "ON films.film_id=films_genres.film_id AND films.film_id=:film_id"
                                  "WHERE films.film_id=: film_id"
                                  "AND films.title=:title"
                                  "AND films.year=:year");
                    query.bindValue(":film_id", list_of_film_id->at(i));
                    query.bindValue(":title", object.value("Title").toString());
                    query.bindValue(":director", decoded_directors.at(j));
                    query.bindValue(":year", object.value("Year").toString());
                    if (query.next()){
                        //const int* gotten_film_id=&list_of_film_id->at(i);
                        //Template_query::select_all_for_record(already_existing_data, &gotten_user_id, gotten_film_id, message.at(0));
                        exist_state=true;
                        break;
                    }
                }
                if(exist_state==true) break;
            }
            //delete list_of_film_id;
            connection.close_db_connection();
            return exist_state;
        }        
        else return 1;

}
Json_creator Select_all_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        int gotten_user_id=Template_query::get_user_id(email, request_code);
        QList<int>* list_of_film_id=new QList<int>();
        Template_query::select_all_for_user(list_of_film_id, &gotten_user_id, request_code);
        if(list_of_film_id->isEmpty()){
            Json_creator result=Json_creator(request_code, true, email);//error_code==true==no_records
            return result;
        }
        QJsonArray rows;
        for (int i=0;i<list_of_film_id->size();i++){
            QJsonObject record=Template_query::select_all_for_record(&gotten_user_id, &list_of_film_id->at(i), request_code);
                rows.push_back(record);
        }
            Json_creator result=Json_creator(request_code, false, email, rows);//error_code==false==records exist
        return result;
}
Json_creator Entry_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        Database_connection connection(request_code);
        //Json_creator* result=new Json_creator(request_code, email);
        if(connection.open_db_connection()){
            QSqlQuery query(connection.get_db());
            int* user_id;
            //QStringList decoded_message=Template_query::decoding_message(message);
            query.prepare("SELECT  name, email FROM users"
                          "WHERE user_email=? "
                          "AND user_password=?");
            query.addBindValue(email);
            query.addBindValue(object.value("Password").toString());
            query.exec();
            if (query.next()){
                QJsonObject* select_all_result;
                Template_query* query_type=Template_query::create_template_query(static_cast<Query_id>(5));
                Json_creator result=query_type->process_request(object);// Select_all_request заполнение информации о всех фильмах для данного пользователя
                result.changing_error_code(false);
                QString user_name=query.value(0).toString();
                result.adding_information("Name", user_name);
                return result;
            }

            connection.close_db_connection();
        }
        return Json_creator(request_code, true, email);
}
Json_creator  Delete_query::process_request( QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        Database_connection connection(request_code);
        if(connection.open_db_connection()){            
            //QStringList decoded_message=Template_query::decoding_message(message);
            QStringList decoded_directors=Template_query::decoding_json_object(object.value(("Directors")));
            QStringList decoded_genres=Template_query::decoding_json_object(object.value("Genres"));
            int gotten_user_id=Template_query::get_user_id(email, request_code);
            int gotten_film_id=gotten_film_id=Template_query::get_film_id(&gotten_user_id, object, request_code);
            QSqlQuery query(connection.get_db());
            query.prepare("DELETE"
                          "FROM films"
                          "WHERE user_id=:gotten_user_id"
                          "AND films.title=:title"
                          "AND films.year=:year"
                          "AND films.rating=:rating"
                          "AND films.status=:status");
            query.bindValue(":gotten_user_id", gotten_user_id);
            query.bindValue(":title", object.value("Title"));
            query.bindValue(":year", object.value("Year"));
            query.bindValue(":rating", object.value("Rating"));
            query.bindValue(":status", object.value("Status"));
            query.exec();
            bool directors_deletion_state=Delete_query::directors_delete(&decoded_directors, &gotten_film_id, request_code);
            bool genres_deletion_state=Delete_query::genres_delete(&decoded_genres, &gotten_film_id, request_code);
            QJsonObject inner_object=object.value("Row").toObject();
            QJsonArray array;
            array.push_back(inner_object);
            if(directors_deletion_state && genres_deletion_state){
                return Json_creator(request_code, false, email, array);
            }
            connection.close_db_connection();           
        }
        return Json_creator(request_code, false, email);//тут можно создавать без array
}
Json_creator Select_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        Database_connection connection(request_code);
        QJsonArray array;
        if(connection.open_db_connection()){
            QSqlQuery query(connection.get_db());
            //QStringList decoded_message=Template_query::decoding_message(message);
            int gotten_user_id=Template_query::get_user_id(email, request_code);
            QList<int>* list_of_film_id=new QList<int>();
            Template_query::select_all_for_user(list_of_film_id, &gotten_user_id, request_code);
            if(list_of_film_id->isEmpty()){
                return Json_creator(request_code, true, email);
            }

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
                query.bindValue(":src_line", object.value("SearchWord"));
                query.exec();               
                if (query.next()){
                    QJsonObject record=Template_query::select_all_for_record(&gotten_user_id, &list_of_film_id->at(i), request_code);
                    array.push_back(record);
                }
            }

        }
        connection.close_db_connection();
        return Json_creator(request_code, array.isEmpty(), email, array);

}
Json_creator  Insert_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        bool exist=Template_query::exist_query(object, request_code);
        QJsonArray array;
        if (exist==0){
            Database_connection connection(request_code);
            if(connection.open_db_connection()){
                QSqlQuery query_films_insert(connection.get_db());
                QStringList decoded_directors=Template_query::decoding_json_object(object.value("Directors"));
                QStringList decoded_genres=Template_query::decoding_json_object(object.value("Genres"));
                int gotten_user_id=Template_query::get_user_id(email, request_code);
                int gotten_film_id;
                query_films_insert.prepare("INSERT INTO films (title, year, rating, status, user_id)"
                                           "VALUES (?, ?, ?, ?, ?)"
                                           "RETURNING film_id");// или film_pk
                query_films_insert.addBindValue(object.value("Title").toString());
                query_films_insert.addBindValue(object.value("Year").toString());
                query_films_insert.addBindValue(object.value("Rating").toString());
                query_films_insert.addBindValue(object.value("Status").toString());
                query_films_insert.addBindValue(gotten_user_id);
                query_films_insert.exec();
                bool films_insert=false;
                if(query_films_insert.next()){
                    films_insert=true;
                }
                gotten_film_id=query_films_insert.value(0).toInt();
                bool directors_insertion_state=Insert_query::directors_insert(&decoded_directors, &gotten_film_id, request_code);
                bool genres_insertion_state=Insert_query::genres_insert(&decoded_genres, &gotten_film_id, request_code);
                if(directors_insertion_state && genres_insertion_state&&films_insert){
                    QJsonObject inner_object=object.value("Row").toObject();
                    array.push_back(inner_object);
                    //return Json_creator(request_code, false, email, array);
                }
                connection.close_db_connection();
            }           
        }
        return Json_creator(request_code, array.isEmpty(), email, array);

}
Json_creator  Registration_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        QString name=object.value("Name").toString();
        qDebug()<<"in registration query email is"<<object.value("Email").toString();
        Database_connection connection(request_code);
        bool row_insert=false;
        if(connection.open_db_connection()){
            qDebug()<<"reg query in open db_connection";
            QSqlQuery query_existance(connection.get_db());
            query_existance.prepare("SELECT user_name FROM users"
                                    "WHERE user_email=?"
                                    "AND user_name=?");
            query_existance.addBindValue(email);
            query_existance.addBindValue(name);
            query_existance.exec();
            if (query_existance.next()){
                qDebug()<<"in reg query such user exist";
                return Json_creator(request_code, true, email, name);
            }
            else{
                qDebug()<<"in reg query such user not exist";
                qDebug()<<"in reg query password is"<<object.value("Password").toString();
                QSqlQuery query(connection.get_db());
                qDebug()<<"prepare is"<<query.prepare("INSERT INTO users (user_id, user_name, user_password, user_email)"
                                                          "VALUES (DEFAULT, :name, :password, :email)"
                                                          "RETURNING user_id");
                query.bindValue(":name", name);
                query.bindValue(":email", email);
                query.bindValue(":password", object.value("Password").toString());
                row_insert=query.exec();

                qDebug()<<"is valid"<<query_existance.isValid();
                qDebug()<<"row insert status is "<<row_insert;
                qDebug()<<"last error"<<query_existance.lastError().databaseText();

                /*if (query.next()){
                    row_insert=true;
                    qDebug()<<query_existance.value(0).toString();
                }*/
            }
            connection.close_db_connection();           
        }
        return Json_creator(request_code, !row_insert, email, name);
}
bool Insert_query::directors_insert(QStringList* directors, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('5');
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; directors->size(); i++){
                QSqlQuery query_directors_update(connection.get_db());
                QSqlQuery query_films_directors_update(connection.get_db());
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
                    QSqlQuery query_gettin_already_exist_director_id(connection.get_db());
                    query_gettin_already_exist_director_id.prepare("SELECT director_id FROM directors"
                                                                   "WHERE director=:director");
                    query_gettin_already_exist_director_id.bindValue(":director", directors->at(i));
                    query_gettin_already_exist_director_id.exec();
                    //while (query_gettin_already_exist_director_id.next()){
                        gotten_director_id=query_gettin_already_exist_director_id.value(0).toInt();
                    //}
                }
                query_films_directors_update.prepare("INSERT INTO films_directors (film_id, director_id)"
                                                     "VALUES (?, ?)");
                query_films_directors_update.addBindValue(*gotten_film_id);
                query_films_directors_update.addBindValue(gotten_director_id);
                query_films_directors_update.exec();
                if(!query_films_directors_update.next())
                    update_state=update_state&&false;
            }

            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}
bool Insert_query::genres_insert(QStringList* genres, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('6');
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; genres->size(); i++){
                QSqlQuery query_getting_genre_id(connection.get_db());
                QSqlQuery query_films_genres_update(connection.get_db());
                int gotten_genre_id;
                query_getting_genre_id.prepare("SELECT genre_id FROM genres"
                                               "WHERE genre=?");
                query_getting_genre_id.addBindValue(genres->at(i));
                query_getting_genre_id.exec();
                if(query_getting_genre_id.next()){
                    gotten_genre_id=query_getting_genre_id.value(0).toInt();
                    query_films_genres_update.prepare("INSERT INTO films_genres (film_id, genre_id)"
                                                      "VALUES (?, ?)"
                                                      "RETURNING film_genre_id");
                    query_films_genres_update.addBindValue(*gotten_film_id);
                    query_films_genres_update.addBindValue(gotten_genre_id);
                    query_films_genres_update.exec();
                    if(!query_films_genres_update.next()){
                         update_state=update_state&&false;
                    }
                }               
            }

            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}
Json_creator  Update_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        QVariantMap map_of_new_data=object.value("Row_new").toObject().toVariantMap();
        QVariantMap map_of_old_data=object.value("Row_old").toObject().toVariantMap();
        QStringList decoded_new_directors=Template_query::decoding_json_object(map_of_new_data.value("Directors").toJsonValue());
        QStringList decoded_new_genres=Template_query::decoding_json_object(map_of_new_data.value("Genres").toJsonValue());
        Database_connection connection(request_code);
        if(connection.open_db_connection()){
            QSqlQuery query_films_update(connection.get_db());
            int gotten_user_id=gotten_user_id=Template_query::get_user_id(email, request_code);
            int gotten_film_id=gotten_film_id=Template_query::get_film_id(&gotten_user_id, object, request_code);
            query_films_update.prepare("UPDATE films"
                                       "SET title=:new_title, year=:new_year, rating=:new_reating, status=:new_status"
                                       "WHERE user_id=:user_id AND film_id=: film_id AND title=:old_title AND year=:old_year AND ratin=old_reating AND  status=:old_status");
            query_films_update.bindValue(":user_id", gotten_user_id);
            query_films_update.bindValue(":film_id", gotten_film_id);
            query_films_update.bindValue(":old_title", map_of_old_data.value("Title").toString());
            query_films_update.bindValue(":old_year", map_of_old_data.value("Year").toString());
            query_films_update.bindValue(":old_rating", map_of_old_data.value("Rating").toString());
            query_films_update.bindValue(":old_status", map_of_old_data.value("Status").toString());
            query_films_update.bindValue(":new_title", map_of_new_data.value("Title").toString());
            query_films_update.bindValue(":new_year", map_of_new_data.value("Year").toString());
            query_films_update.bindValue(":new_rating", map_of_new_data.value("Rating").toString());
            query_films_update.bindValue(":new_status", map_of_new_data.value("Status").toString());
            query_films_update.exec();
            bool films_update=false;
            if(query_films_update.next()){
                films_update=true;
            }
            bool directors_updation_state=Update_query::directors_update(&decoded_new_directors, &gotten_film_id, request_code);
            bool genres_updation_state=Update_query::genres_update(&decoded_new_genres, &gotten_film_id, request_code);
            if(directors_updation_state && genres_updation_state&&films_update){
                connection.close_db_connection();
                return Json_creator(request_code, false, email, object.value("Row_new").toObject(), object.value("Row_old").toObject());
            }           
            connection.close_db_connection();
        }
        return Json_creator(request_code, true, email);
}
bool Update_query::directors_update(QStringList* directors, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('7');
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; directors->size(); i++){
                QSqlQuery query_directors_update(connection.get_db());
                QSqlQuery query_films_directors_update(connection.get_db());
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
                    QSqlQuery query_gettin_already_exist_director_id(connection.get_db());
                    query_gettin_already_exist_director_id.prepare("SELECT director_id FROM directors"
                                                                   "WHERE director=:director");
                    query_gettin_already_exist_director_id.bindValue(":director", directors->at(i));
                    query_gettin_already_exist_director_id.exec();
                    //if(query_gettin_already_exist_director_id.next()){
                        gotten_director_id=query_gettin_already_exist_director_id.value(0).toInt();
                    //}
                }
                query_films_directors_update.prepare("UPDATE films_directors"
                                                     "SET director_id=:director_id"
                                                     "WHERE film_id=:film_id"
                                                     "RETURNING film_director_id");
                query_films_directors_update.bindValue(":film_id", *gotten_film_id);
                query_films_directors_update.bindValue(":director_id", gotten_director_id);
                query_films_directors_update.exec();
                if(!query_films_directors_update.next())
                        update_state=update_state&&false;
            }
            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}
bool Update_query::genres_update(QStringList* genres, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('6');
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; genres->size(); i++){
                QSqlQuery query_getting_genre_id(connection.get_db());
                QSqlQuery query_films_genres_update(connection.get_db());
                int gotten_genre_id;
                query_getting_genre_id.prepare("SELECT genre_id FROM genres"
                                               "WHERE genre=?");
                query_getting_genre_id.addBindValue(genres->at(i));
                query_getting_genre_id.exec();
                if(query_getting_genre_id.next()){
                    gotten_genre_id=query_getting_genre_id.value(0).toInt();
                    query_films_genres_update.prepare("UPDATE films_genres"
                                                      "SET genre_id=:genre_id"
                                                      "WHERE film_i:film_id"
                                                      "RETURNING film_genre_id");
                    query_films_genres_update.bindValue(":film_id", *gotten_film_id);
                    query_films_genres_update.bindValue(":genre_id", gotten_genre_id);
                    query_films_genres_update.exec();
                    if(!query_films_genres_update.next())
                         update_state=update_state&&false;
                }

            }
            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}

bool Delete_query::directors_delete(QStringList* directors, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('9');
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; directors->size(); i++){
                QSqlQuery query_films_directors_delete(connection.get_db());
                int gotten_director_id;
                QSqlQuery query_gettin_already_exist_director_id(connection.get_db());
                query_gettin_already_exist_director_id.prepare("SELECT director_id FROM directors"
                                                               "WHERE director=:director");
                query_gettin_already_exist_director_id.bindValue(":director", directors->at(i));
                query_gettin_already_exist_director_id.exec();
                //if(query_gettin_already_exist_director_id.next()){
                   gotten_director_id=query_gettin_already_exist_director_id.value(0).toInt();
                //}
                query_films_directors_delete.prepare("DELETE FROM  films_directors"
                                                     "WHERE film_id=:film_id AND director_id=:director_id"
                                                     "RETURNING film_director_id");
                query_films_directors_delete.bindValue(":film_id", *gotten_film_id);
                query_films_directors_delete.bindValue(":director_id", gotten_director_id);
                query_films_directors_delete.exec();
                if(!query_films_directors_delete.next())
                    update_state=update_state&&false;
            }

            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}
bool Delete_query::genres_delete(QStringList* genres, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        QString type("10");
        type_connection.QString::push_back(type);
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; genres->size(); i++){
                QSqlQuery query_films_genres_delete(connection.get_db());
                int gotten_genre_id;
                QSqlQuery query_gettin_already_exist_genre_id(connection.get_db());
                query_gettin_already_exist_genre_id.prepare("SELECT genre_id FROM genress"
                                                               "WHERE genre=:genre");
                query_gettin_already_exist_genre_id.bindValue(":genre", genres->at(i));
                query_gettin_already_exist_genre_id.exec();
                //if(query_gettin_already_exist_genre_id.next()){
                   gotten_genre_id=query_gettin_already_exist_genre_id.value(0).toInt();
                //}
                query_films_genres_delete.prepare("DELETE FROM  films_genres"
                                                  "WHERE film_id=:film_id AND genre_id=:genre_id"
                                                  "RETIRNING film_genre_id");
                query_films_genres_delete.bindValue(":film_id", *gotten_film_id);
                query_films_genres_delete.bindValue(":director_id", gotten_genre_id);
                query_films_genres_delete.exec();
                if(!query_films_genres_delete.next())
                    update_state=update_state&&false;
            }
            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}
