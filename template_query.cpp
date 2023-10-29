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
    // вместо подобных нижестоящему свитчей используй си-массивы
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
            qDebug()<<"in decoding jdon object element is"<<object_array.at(i).toString();
            decoded_object.push_back(object_array.at(i).toString());
        }
        return decoded_object;
}

int Template_query::get_user_id(const QString& email, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_get_user_id");
        Database_connection connection(type_connection);
        int gotten_user_id=-1;
        if(connection.open_db_connection()){
            QSqlQuery query_get_user_id(connection.get_db());
            query_get_user_id.prepare("SELECT user_id FROM users WHERE user_email=:email");
            query_get_user_id.bindValue(":email", email);
            query_get_user_id.exec();
            if(query_get_user_id.next()){
                gotten_user_id=query_get_user_id.value(0).toInt();
            }
            connection.close_db_connection();
        }
        qDebug()<<"in get user_id id is:"<<gotten_user_id;
        return gotten_user_id;
}

int Template_query::get_film_id(const QString& email, const QJsonObject &object, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_get_film_id");

        Database_connection connection(type_connection);
        int gotten_film_id=-1;
        if(connection.open_db_connection()){
            QVariantMap object_map=object.toVariantMap();
            QJsonObject row;
            if(object_map.contains("Row")){
                row=object_map.value("Row").toJsonObject();
            }
            else if(object_map.contains("Row_old")){
                row=object_map.value("Row_old").toJsonObject();
            }
            else return gotten_film_id;

            QStringList decoded_directors=Template_query::decoding_json_object(row.value("Directors"));
            QStringList decoded_genres=Template_query::decoding_json_object(row.value("Genres"));

            QSqlQuery query_get_film_id(connection.get_db());
            query_get_film_id.prepare("SELECT films.film_id FROM films "
                                      "INNER JOIN "
                                      "(films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id AND directors.director=:director) "
                                              "ON films.film_id=films_directors.film_id "

                                      "INNER JOIN "
                                      "(films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id AND genres.genre=:genre) "
                                      "ON films.film_id=films_genres.film_id "
                                      "WHERE films.title=:title AND films.year=:year AND films.rating=:rating AND films.status=:status "
                                      "AND films.user_id=(SELECT users.user_id "
                                                         "FROM users "
                                                         "WHERE users.user_email=:email)");
            query_get_film_id.bindValue(":title", row.value("Title").toString());
            query_get_film_id.bindValue(":year", row.value("Year").toString());
            query_get_film_id.bindValue(":director", decoded_directors.at(0));
            query_get_film_id.bindValue(":genre", decoded_genres.at(0));
            query_get_film_id.bindValue(":rating", row.value("Rating").toString());
            query_get_film_id.bindValue(":status", row.value("Status").toString());
            query_get_film_id.bindValue(":email", email);
            query_get_film_id.exec();
            qDebug()<<query_get_film_id.lastError();

            if(query_get_film_id.next()){
                gotten_film_id=query_get_film_id.value(0).toInt();
            }
            connection.close_db_connection();
        }
        qDebug()<<"in get film id "<<gotten_film_id;
        return gotten_film_id;
}
QList<int>* Template_query::select_all_for_user(const QString& email, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_select_all_for_user");
        Database_connection connection(type_connection);
        QList<int>* films_id=new QList<int>();
        if(connection.open_db_connection()){

            QSqlQuery query_select_all_for_user(connection.get_db());
            query_select_all_for_user.prepare("SELECT film_id FROM films "
                                              "WHERE films.user_id=(SELECT users.user_id "
                                                                    "FROM users "
                                                                    "WHERE users.user_email=:email) ");
            query_select_all_for_user.bindValue(":email", email);
            qDebug()<<"last error after prepare in query select all for user"<<query_select_all_for_user.lastError();
            query_select_all_for_user.exec();
            qDebug()<<"last error in query select all"<<query_select_all_for_user.lastError();
            while (query_select_all_for_user.next()){
                films_id->push_back(query_select_all_for_user.value(0).toInt());
                //qDebug()<<"in select all for user film_id is"<<query_select_all_for_user.value(0).toInt();
            }
            //connection.close_db_connection();
        }
        return films_id;
}
QJsonObject Template_query::select_all_for_record(int film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_select_all_for_record");
        Database_connection connection(type_connection);
        QJsonObject record;
        if(connection.open_db_connection()){
            QSqlQuery query_title_year_rating_status(connection.get_db());
            QSqlQuery query_directors(connection.get_db());
            QSqlQuery query_genres(connection.get_db());
            query_title_year_rating_status.prepare("SELECT films.title, films.year, films.rating, films.status "
                                                   "FROM films "
                                                   "WHERE films.film_id=:film_id");
            query_title_year_rating_status.bindValue(":film_id", film_id);
            query_title_year_rating_status.exec();
            if (query_title_year_rating_status.next()){

                record["Title"]=query_title_year_rating_status.value(0).toString();
                record["Year"]=query_title_year_rating_status.value(1).toString();//заполнение года
                record["Rating"]=query_title_year_rating_status.value(2).toString();//заполнение рейтинга
                record["Status"]=query_title_year_rating_status.value(3).toString();//заполнение статуса
                query_directors.prepare("SELECT  directors.director "
                                        "FROM films "
                                        "INNER JOIN "
                                        "films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id "
                                        "ON films.film_id=films_directors.film_id AND films.film_id=:film_id");
                query_directors.bindValue(":film_id", film_id);
                query_directors.exec();
                qDebug()<<"last error in select all directors for record"<<query_directors.lastError();
                query_genres.prepare("SELECT  genres.genre "
                                     "FROM films "
                                     "INNER JOIN "
                                     "films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id "
                                     "ON films.film_id=films_genres.film_id AND films.film_id=:film_id");
                query_genres.bindValue(":film_id", film_id);
                query_genres.exec();
                qDebug()<<"last error in select all genres for record"<<query_genres.lastError();
                QJsonArray directors;
                QJsonArray genres;
                while(query_directors.next()){
                    directors.push_back(query_directors.value(0).toString());//counter=0
                    qDebug()<<"in select all for record director is"<<query_directors.value(0).toString();
                }
                while(query_genres.next()){
                    genres.push_back(query_genres.value(0).toString());//counter=0
                }
                record["Directors"]=directors;
                record["Genres"]=genres;
            }
            //connection.close_db_connection();
        }
        return record;
}
/*QJsonObject Template_query::select_all_for_record(const QString& email, int number_of_row, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back('_');
        type_connection.QString::push_back('3');
        Database_connection connection(type_connection);
        //QString email=object.value("Email").toString();
        QJsonObject record;
        if(connection.open_db_connection()){
            QSqlQuery query_title_year_rating_status(connection.get_db());
            QSqlQuery query_directors(connection.get_db());
            QSqlQuery query_genres(connection.get_db());
            query_title_year_rating_status.prepare("SELECT films.film_id, films.title, films.year, films.rating, films.status "
                                                   "FROM films "
                                                   "WHERE films.user_id="
                                                                        "(SELECT users.user_id "
                                                                        "FROM users "
                                                                        "WHERE users.user_email=:email) "
                                                   "ORDER BY films.film_id "
                                                   "LIMIT 1 OFFSET :number_of_row");
            query_title_year_rating_status.bindValue(":email", email);
            query_title_year_rating_status.bindValue(":number_of_row", number_of_row);
            query_title_year_rating_status.exec();
            if (query_title_year_rating_status.next()){
                int film_id=query_title_year_rating_status.value(0).toInt();
                record["Title"]=query_title_year_rating_status.value(1).toString();
                record["Year"]=query_title_year_rating_status.value(2).toString();//заполнение года
                record["Rating"]=query_title_year_rating_status.value(3).toString();//заполнение рейтинга
                record["Status"]=query_title_year_rating_status.value(4).toString();//заполнение статуса
                query_directors.prepare("SELECT  directors.director "
                                        "FROM films "
                                        "INNER JOIN "
                                        "films_directors INNER JOIN directors ON films_directors.director_id=directors.director_id "
                                        "ON films.film_id=films_directors.film_id AND films.film_id=:film_id");
                query_directors.bindValue(":film_id", film_id);
                query_directors.exec();
                qDebug()<<"last error in select all directors for record"<<query_directors.lastError();
                query_genres.prepare("SELECT  genres.genre "
                                     "FROM films "
                                     "INNER JOIN "
                                     "films_genres INNER JOIN genres ON films_genres.genre_id=genres.genre_id "
                                     "ON films.film_id=films_genres.film_id AND films.film_id=:film_id");
                query_genres.bindValue(":film_id", film_id);
                query_genres.exec();
                qDebug()<<"last error in select all genres for record"<<query_genres.lastError();
                QJsonArray directors;
                QJsonArray genres;
                while(query_directors.next()){
                    directors.push_back(query_directors.value(0).toString());//counter=0
                    qDebug()<<"in select all for record director is"<<query_directors.value(0).toString();
                }
                while(query_genres.next()){
                    genres.push_back(query_genres.value(0).toString());//counter=0
                }
                record["Directors"]=directors;
                record["Genres"]=genres;
            }
            connection.close_db_connection();
        }
        return record;
}*/
 bool Template_query::exist_query(const QJsonObject& object, const QString parent_type_connection){//разобрать эту
        bool exist_state=false;
        //QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        QJsonObject object_row=object.value("Row").toObject();
        //int gotten_user_id=Template_query::get_user_id(email, request_code);
        QStringList decoded_directors=Template_query::decoding_json_object(object_row.value("Directors"));
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_exist_query");
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){           
                for (int j=0;j<decoded_directors.size();j++){
                    QSqlQuery query(connection.get_db());
                    query.prepare("SELECT films.film_id "
                                  "FROM "
                                  "films INNER JOIN "
                                  "films_directors  INNER JOIN "
                                  "directors ON films_directors.director_id=directors.director_id  AND directors.director=:director "
                                  "ON films.film_id=films_directors.film_id "

                                  "WHERE films.user_id=(SELECT users.user_id "
                                                        "FROM users "
                                                        "WHERE users.user_email=:email)"
                                  "AND films.title=:title AND films.year=:year ");
                    query.bindValue(":year",  object_row.value("Year").toString());
                    query.bindValue(":title",  object_row.value("Title").toString());
                    query.bindValue(":director", decoded_directors.at(j));
                    query.bindValue(":email", email);
                    query.exec();
                    qDebug()<<"last error in exist query"<<query.lastError();
                    bool intermediate_exist_state=false;
                    if((intermediate_exist_state=query.next())){
                        qDebug()<<"last error"<<query.lastError();
                        qDebug()<<"intermidiate exist state is"<<intermediate_exist_state;
                        exist_state=true;
                        break;
                    }
                }
                connection.close_db_connection();
                return exist_state;
        }        
        else return true;

}
 int Template_query::count_of_films_for_user(const QString& email, const QString parent_type_connection){
        int count=0;
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_count_of_films_for_user");
        Database_connection connection(type_connection);
        if(connection.open_db_connection()){
            QSqlQuery query(connection.get_db());
            query.prepare("SELECT COUNT(title) "
                          "FROM films "
                          "WHERE films.user_id="
                                               "(SELECT users.user_id "
                                               "FROM users "
                                               "WHERE users.user_email=:email)"
                          );
            query.bindValue(":email", email);
            query.exec();
            qDebug()<<"last error count of films for user"<<query.lastError();
            if(query.next()){
                    count=query.value(0).toInt();
            }
            connection.close_db_connection();
        }
        return count;
 }
Json_creator Select_all_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        //int count_of_films_for_user=Template_query::count_of_films_for_user(email, request_code);
        //int gotten_user_id=Template_query::get_user_id(email, request_code);
        QList<int>* list_of_film_id=select_all_for_user(email, request_code);
        if(list_of_film_id->size()==0){
            Json_creator result=Json_creator(request_code, true, email);//error_code==true==no_records
            return result;
        }
        QJsonArray rows;
        for (int i=0;i<list_of_film_id->size();i++){
            QJsonObject record=Template_query::select_all_for_record(list_of_film_id->at(i), request_code);
            rows.push_back(record);
        }
        Json_creator result=Json_creator(request_code, false, email, rows);//error_code==false==records exist
        return result;
}
Json_creator Entry_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        Database_connection connection(request_code);
        if(connection.open_db_connection()){
            QSqlQuery query(connection.get_db());
            int* user_id;
            query.prepare("SELECT  user_name, user_email FROM users "
                          "WHERE user_email=:email "
                          "AND user_password=:password");
            query.bindValue(":email", email);
            query.bindValue(":password", object.value("Password").toString());
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
        QJsonObject object_row=object.value("Row").toObject();
        bool deletion_state=false;
        Database_connection connection(request_code);
        if(connection.open_db_connection()){
            //QStringList decoded_directors=Template_query::decoding_json_object(object_row.value("Directors"));
            //decoded_directors.sort();
            int film_id=Template_query::get_film_id(email, object, request_code);
            //for(int i=0; i<decoded_directors.size();i++){
                QSqlQuery query(connection.get_db());
                query.prepare( "DELETE FROM films WHERE films.film_id=:film_id "
                               "RETURNING films.film_id");
                query.bindValue(":film_id", film_id);
                qDebug()<<"last error in delete after prepare"<<query.lastError();
                query.exec();
                qDebug()<<"last error in delete"<<query.lastError();
                if(query.next()){
                    qDebug()<<"in delete film_id is"<<query.value(0).toString();
                    deletion_state=true;
                    //break;
                }
            //}
            connection.close_db_connection();           
        }
        QJsonObject inner_object=object.value("Row").toObject();
        QJsonArray array;
        array.push_back(inner_object);
        return Json_creator(request_code, !deletion_state, email, array);//тут можно создавать без array
}
Json_creator Select_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        qDebug()<<"in select query search word is"<<object.value("SearchWord").toString();
        Database_connection connection(request_code);
        QJsonArray array;
        if(connection.open_db_connection()){


            QList<int>* list_of_film_id=Template_query::select_all_for_user(email, request_code);
            qDebug()<<"list of films id size is"<<list_of_film_id->size();
            if(list_of_film_id->size()==0){

                return Json_creator(request_code, true, email);
            }

            for(int i=0; i<list_of_film_id->size();i++){
                QSqlQuery query(connection.get_db());
                query.prepare("SELECT films.film_id, films.title, directors.director, genres.genre, films.year, films.rating, films.status "
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
                query.bindValue(":src_line", object.value("SearchWord").toString());
                query.bindValue(":film_id", list_of_film_id->at(i));
                qDebug()<<"last error after prepare"<<query.lastError();
                query.exec();
                qDebug()<<"last error after exec in select query"<<query.lastError();
                if (query.next()){
                    int film_id=query.value(0).toInt();
                    qDebug()<<"in select query title is"<<query.value(1).toString();
                    QJsonObject record=Template_query::select_all_for_record(film_id, request_code);
                    array.push_back(record);
                }
            }
            connection.close_db_connection();
        }

        return Json_creator(request_code, array.isEmpty(), email, array);

}
Json_creator  Insert_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        QJsonObject object_row=object.value("Row").toObject();       
        bool exist=Template_query::exist_query(object, request_code);
        bool film_insertion_state=false;
        bool directors_insertion_state=false;
        bool genres_insertion_state=false;
        bool film_insertion_commit_state=false;
        qDebug()<<"in insert query exist state  is"<<exist;
        QJsonArray array;
        if (exist==0){
            Database_connection connection(request_code);
            if(connection.open_db_connection()&&connection.start_db_transaction()){
                QSqlQuery query_films_insert(connection.get_db());
                int gotten_film_id;
                query_films_insert.prepare("INSERT INTO films (user_id, title, year, rating, status, film_id)"
                                           "VALUES ((SELECT users.user_id FROM users  WHERE users.user_email=:email), :title, :year, :rating, :status, DEFAULT)"
                                           "RETURNING film_id");// или film_pk
                query_films_insert.bindValue(":title", object_row.value("Title").toString());
                query_films_insert.bindValue(":year", object_row.value("Year").toString());
                query_films_insert.bindValue(":rating", object_row.value("Rating").toString());
                query_films_insert.bindValue(":status", object_row.value("Status").toString());
                query_films_insert.bindValue(":email", email);
                query_films_insert.exec();
                qDebug()<<"last error films insert"<<query_films_insert.lastError();
                film_insertion_commit_state=connection.do_db_commit();
                film_insertion_state=query_films_insert.next();
                if(film_insertion_commit_state){
                    if(film_insertion_state){
                        gotten_film_id=query_films_insert.value(0).toInt();
                        directors_insertion_state=Insert_query::directors_insert(object, gotten_film_id, request_code);
                        genres_insertion_state=Insert_query::genres_insert(object, gotten_film_id, request_code);
                        qDebug()<<"directors insert is"<<directors_insertion_state;
                        qDebug()<<"genres insert is"<<genres_insertion_state;
                        film_insertion_state=directors_insertion_state&&genres_insertion_state;
                        if(!directors_insertion_state) connection.do_db_rollback();
                    }
                }
                else connection.do_db_rollback();
                connection.close_db_connection();
            }           
        }
        QJsonObject inner_object=object.value("Row").toObject();
        array.push_back(inner_object);
        bool insert_state=film_insertion_state&&directors_insertion_state&&genres_insertion_state;
        return Json_creator(request_code, !(!exist&&insert_state), email, array);

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
            query_existance.prepare("SELECT user_name FROM users "
                                    "WHERE user_name=:name "
                                    "AND user_email=:email");
            query_existance.bindValue(":email", email);
            query_existance.bindValue(":name", name);
            bool select=query_existance.exec();
            qDebug()<<"last error"<<query_existance.lastError().databaseText();
            if (query_existance.next()){
                qDebug()<<"in reg query such user exist, user name is"<<query_existance.value(0).toString();
                return Json_creator(request_code, true, email, name);
            }
            else{
                qDebug()<<"in reg query such user not exist";
                QSqlQuery query(connection.get_db());
                qDebug()<<"prepare is"<<query.prepare("INSERT INTO users (user_id, user_name, user_password, user_email)"
                                                          "VALUES (DEFAULT, :name, :password, :email)"
                                                          "RETURNING user_id");
                query.bindValue(":name", name);
                query.bindValue(":email", email);
                query.bindValue(":password", object.value("Password").toString());
                row_insert=query.exec();                
                qDebug()<<"last error"<<query.lastError().databaseText();               
            }
            connection.close_db_connection();           
        }
        return Json_creator(request_code, !row_insert, email, name);
}
bool Insert_query::directors_insert(QJsonObject &object, int film_id, const QString parent_type_connection){
        QString type_connection=parent_type_connection;
        type_connection+="_directors_insert";
        //type_connection.QString::push_back('5');
        Database_connection connection(type_connection);
        QString email=object.value("Email").toString();
        QJsonObject object_row=object.value("Row").toObject();
        QStringList directors=Template_query::decoding_json_object(object_row.value("Directors"));
        bool insert_state=true;
        //bool insert_director_state;
        if(connection.open_db_connection()){
            for(int i=0; i<directors.size(); i++){               
                QSqlQuery query_gettin_already_exist_director_id(connection.get_db());
                query_gettin_already_exist_director_id.prepare("SELECT director_id FROM directors "
                                                               "WHERE director=:director ");
                query_gettin_already_exist_director_id.bindValue(":director", directors.at(i));
                query_gettin_already_exist_director_id.exec();
                qDebug()<<"last error getting already exist director insert"<<query_gettin_already_exist_director_id.lastError();
                if (query_gettin_already_exist_director_id.next()){
                    insert_state=insert_state&&Insert_query::insertion_of_existing_director(directors.at(i), film_id, type_connection);
                    if(!insert_state) return insert_state;
                }
                else{
                    insert_state=insert_state&&Insert_query::insertion_of_not_existing_director(directors.at(i), film_id, type_connection);
                    if(!insert_state) return insert_state;
                }                
            }
            connection.close_db_connection();
        }
        else return false;
        return insert_state;
}
bool Insert_query::insertion_of_existing_director(const QString& director, int film_id, const QString parent_type_connection){
        QString type_connection=parent_type_connection;
        type_connection+="_insertion_of_existing_director";
        Database_connection connection(type_connection);
        bool insertion_state=false;
        //bool insertion_commit_state=false;
        if(connection.open_db_connection()&&connection.start_db_transaction()){
            QSqlQuery query_if_director_is_exist (connection.get_db());
            query_if_director_is_exist.prepare("INSERT INTO films_directors (film_id, director_id, film_director_id)"
                                                   "VALUES( :film_id, "
                                                   "(SELECT director_id "
                                                   "FROM directors "
                                                   "WHERE director=:director "
                                                   "LIMIT 1), "
                                                   "DEFAULT)"
                                                   "RETURNING film_director_id");
            query_if_director_is_exist.bindValue(":film_id", film_id);
            query_if_director_is_exist.bindValue(":director", director);
            query_if_director_is_exist.exec();
            qDebug()<<"last error query_if_director_is_exist"<<query_if_director_is_exist.lastError();
            insertion_state=query_if_director_is_exist.next()&&connection.do_db_commit();
            if(!insertion_state){
                connection.do_db_rollback();
            }
            connection.close_db_connection();
        }
        return insertion_state;
}
bool Insert_query::insertion_of_not_existing_director(const QString& director, int film_id, const QString parent_type_connection){
        QString type_connection=parent_type_connection;
        type_connection+="_insertion_of_not_existing_director";
        Database_connection connection(type_connection);
        bool insertion_state=false;
        //bool insertion_commit_state=false;
        if(connection.open_db_connection()&&connection.start_db_transaction()){
            QSqlQuery query_if_director_is_not_exist(connection.get_db());
            query_if_director_is_not_exist.prepare("WITH last_director_id AS( "
                                                   "INSERT INTO directors(director_id, director) "
                                                   "VALUES(DEFAULT, :director) RETURNING director_id) "
                                                   "INSERT INTO films_directors(film_id, director_id, film_director_id) "
                                                   "VALUES( :film_id, "
                                                   "(SELECT director_id FROM last_director_id LIMIT 1), DEFAULT)"
                                                   "RETURNING film_director_id");
            query_if_director_is_not_exist.bindValue(":film_id", film_id);
            query_if_director_is_not_exist.bindValue(":director", director);
            query_if_director_is_not_exist.exec();
            qDebug()<<"last error query_if_director_is_exist"<<query_if_director_is_not_exist.lastError();
            insertion_state=query_if_director_is_not_exist.next()&&connection.do_db_commit();
            if(!insertion_state){
                connection.do_db_rollback();
            }

            connection.close_db_connection();
        }
        return insertion_state;
}

bool Insert_query::genres_insert(QJsonObject &object, int film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_genres_insert");
        Database_connection connection(type_connection);
        //QString email=object.value("Email").toString();
        QJsonObject object_row=object.value("Row").toObject();
        QStringList genres=Template_query::decoding_json_object(object_row.value("Genres"));
        bool insert_state=true;
        //bool insert_genre_state=false;
        if(connection.open_db_connection()){
            for(int i=0; i<genres.size(); i++){
                if(connection.start_db_transaction()){
                    QSqlQuery query_film_genre_insert(connection.get_db());
                    query_film_genre_insert.prepare("INSERT INTO films_genres(film_id, genre_id, film_genre_id) "
                                                    "VALUES(:film_id, "
                                                    "(SELECT genre_id FROM genres WHERE genre=:genre), DEFAULT)"
                                                    "RETURNING genre_id");
                    query_film_genre_insert.bindValue(":film_id", film_id);
                    query_film_genre_insert.bindValue(":genre", genres.at(i));
                    query_film_genre_insert.exec();
                    qDebug()<<"last error query_film_genre_insert"<<query_film_genre_insert.lastError();
                    bool insert_genre_state=query_film_genre_insert.next()&&connection.do_db_commit();
                    if(!insert_genre_state){
                        connection.do_db_rollback();
                    }
                    insert_state=insert_state&&insert_genre_state;
                    if(!insert_state) return insert_state;
                }
                else return false;
            }
            connection.close_db_connection();
        }
        else return false;;
        return insert_state;
}
bool Insert_query::genres_insert(QStringList* genres, int film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_genres_insert");
        Database_connection connection(type_connection);
        bool insert_state=true;
        //bool insert_genre_state=false;
        if(connection.open_db_connection()){
            for(int i=0; i<genres->size(); i++){
                if(connection.start_db_transaction()){
                    QSqlQuery query_film_genre_insert(connection.get_db());
                    query_film_genre_insert.prepare("INSERT INTO films_genres(film_id, genre_id, film_genre_id) "
                                                    "VALUES(:film_id, "
                                                    "(SELECT genre_id FROM genres WHERE genre=:genre), DEFAULT)"
                                                    "RETURNING genre_id");
                    query_film_genre_insert.bindValue(":film_id", film_id);
                    query_film_genre_insert.bindValue(":genre", genres->at(i));
                    query_film_genre_insert.exec();
                    qDebug()<<"last error query_film_genre_insert"<<query_film_genre_insert.lastError();
                    bool insert_genre_state=query_film_genre_insert.next()&&connection.do_db_commit();
                    if(!insert_genre_state){
                        connection.do_db_rollback();
                    }
                    insert_state=insert_state&&insert_genre_state;
                    if(!insert_state) return insert_state;
                }
                else return false;
            }
            connection.close_db_connection();
        }
        else return false;;
        return insert_state;
}
Json_creator  Update_query::process_request(QJsonObject &object){
        QString request_code=object.value("RequestCode").toString();
        QString email=object.value("Email").toString();
        QVariantMap map_of_new_data=object.value("Row_new").toObject().toVariantMap();
        QVariantMap map_of_old_data=object.value("Row_old").toObject().toVariantMap();
        QStringList decoded_new_directors=Template_query::decoding_json_object(map_of_new_data.value("Directors").toJsonValue());
        QStringList decoded_new_genres=Template_query::decoding_json_object(map_of_new_data.value("Genres").toJsonValue());
        QStringList decoded_old_directors=Template_query::decoding_json_object(map_of_old_data.value("Directors").toJsonValue());
        QStringList decoded_old_genres=Template_query::decoding_json_object(map_of_old_data.value("Genres").toJsonValue());
        bool film_updation_state=false;
        bool directors_updation_state=false;
        bool genres_updation_state=false;
        bool film_updation_commit_state=false;
        Database_connection connection(request_code);
        if(connection.open_db_connection()&&connection.start_db_transaction()){
            QSqlQuery query_films_update(connection.get_db());
            int gotten_film_id=Template_query::get_film_id(email, object, request_code);
            query_films_update.prepare("UPDATE films "
                                       "SET title=:new_title, year=:new_year, rating=:new_rating, status=:new_status "
                                       "WHERE film_id=:film_id "
                                       "RETURNING film_id");
            query_films_update.bindValue(":film_id", gotten_film_id);
            query_films_update.bindValue(":new_title", map_of_new_data.value("Title").toString());
            query_films_update.bindValue(":new_year", map_of_new_data.value("Year").toString());
            query_films_update.bindValue(":new_rating", map_of_new_data.value("Rating").toString());
            query_films_update.bindValue(":new_status", map_of_new_data.value("Status").toString());
            query_films_update.exec();
             qDebug()<<"last error in query films_update"<<query_films_update.lastError();
            film_updation_commit_state=connection.do_db_commit();
            film_updation_state=query_films_update.next();
            if(film_updation_commit_state){
                if(film_updation_state){
                    //gotten_film_id=query_films_.value(0).toInt();
                    directors_updation_state=Update_query::directors_update(&decoded_old_directors, &decoded_new_directors, gotten_film_id, request_code);
                    genres_updation_state=Update_query::genres_update(&decoded_old_genres, &decoded_new_genres, gotten_film_id, request_code);
                    qDebug()<<"directors insert is"<<directors_updation_state;
                    qDebug()<<"genres insert is"<<genres_updation_state;
                    film_updation_state=directors_updation_state&&genres_updation_state;
                    if(!directors_updation_state||!genres_updation_state) connection.do_db_rollback();
                }
            }
            else connection.do_db_rollback();
            connection.close_db_connection();
        }
        //QJsonArray roe;
        QJsonObject row_new=object.value("Row_new").toObject();
        QJsonObject row_old=object.value("Row_old").toObject();
        //array.push_back(inner_object);
        bool update_state=film_updation_state&&directors_updation_state&&genres_updation_state;
        return Json_creator(request_code, !update_state, email, row_new, row_old);
}
bool Update_query::directors_update(QStringList* old_directors, QStringList* directors, int film_id, const QString parent_type_connection){
        QString type_connection=parent_type_connection;
        type_connection+="_directors_update";
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            bool deletion_state=Delete_query::directors_delete(old_directors, &film_id, type_connection);
            if(deletion_state){
                for(int i=0; i<directors->size(); i++){
                    qDebug()<<"in directors update cycle";
                    QSqlQuery query_gettin_already_exist_director_id(connection.get_db());
                    query_gettin_already_exist_director_id.prepare("SELECT director_id FROM directors "
                                                                    "WHERE director=:director ");
                    query_gettin_already_exist_director_id.bindValue(":director", directors->at(i));
                    query_gettin_already_exist_director_id.exec();
                    qDebug()<<"last error getting already exist director update"<<query_gettin_already_exist_director_id.lastError();
                    if (query_gettin_already_exist_director_id.next()){
                        update_state=update_state&&Insert_query::insertion_of_existing_director(directors->at(i), film_id, type_connection);
                        if(!update_state) break;
                    }
                    else{
                        update_state=update_state&&Insert_query::insertion_of_not_existing_director(directors->at(i), film_id, type_connection);
                        if(!update_state) break;
                    }
                }
            }
            else update_state=false;
            connection.close_db_connection();
        }
        else return false;
        return update_state;
}

bool Update_query::genres_update(QStringList* old_genres, QStringList* genres, int film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_genres_update");
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            bool deletion_state=Delete_query::genres_delete(old_genres, &film_id, type_connection);
            if(deletion_state){
                    update_state=update_state&&Insert_query::genres_insert(genres, film_id, type_connection);
            }
            else return false;
            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}

bool Delete_query::directors_delete(QStringList* directors, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_directors_delete");
        Database_connection connection(type_connection);
        bool delete_state=true;
        if(connection.open_db_connection()){
            for(int i=0; i<directors->size(); i++){
                QSqlQuery query_films_directors_delete(connection.get_db());
                query_films_directors_delete.prepare("DELETE FROM  films_directors "
                                                     "WHERE film_id=:film_id AND director_id=(SELECT director_id FROM directors "
                                                                                              "WHERE director=:director) "
                                                     "RETURNING film_director_id");
                query_films_directors_delete.bindValue(":film_id", *gotten_film_id);
                query_films_directors_delete.bindValue(":director", directors->at(i));
                query_films_directors_delete.exec();
                if(!query_films_directors_delete.next())
                    delete_state=delete_state&&false;
                qDebug()<<"in directors delete film_director_id is"<<query_films_directors_delete.value(0).toInt();
                qDebug()<<"last error in query_films_directors_delete"<<query_films_directors_delete.lastError();

            }

            connection.close_db_connection();
        }
        else delete_state=false;
        return delete_state;
}
bool Delete_query::genres_delete(QStringList* genres, int* gotten_film_id, const QString parent_type_connection){
        QString type_connection(parent_type_connection);
        type_connection.QString::push_back("_genres_delete");
        //QString type("10");
        //type_connection.QString::push_back(type);
        Database_connection connection(type_connection);
        bool update_state=true;
        if(connection.open_db_connection()){
            for(int i=0; i<genres->size(); i++){
                QSqlQuery query_films_genres_delete(connection.get_db());
                query_films_genres_delete.prepare("DELETE FROM  films_genres "
                                                  "WHERE film_id=:film_id AND genre_id=(SELECT genre_id FROM genres "
                                                                                        "WHERE genre=:genre) "
                                                  "RETURNING film_genre_id");
                query_films_genres_delete.bindValue(":film_id", *gotten_film_id);
                query_films_genres_delete.bindValue(":genre", genres->at(i));
                query_films_genres_delete.exec();
                qDebug()<<"in genres delete film_genre_id is"<<query_films_genres_delete.value(0).toInt();
                qDebug()<<"last error in query_films_genres_delete"<<query_films_genres_delete.lastError();
                if(!query_films_genres_delete.next())
                    update_state=update_state&&false;
            }
            connection.close_db_connection();
        }
        else update_state=false;
        return update_state;
}

// не вникал в детали, но выглядит как несколько подобных по структуре функций.
// мб можно сделать лаконичнее


