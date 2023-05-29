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
QStringList* Template_query::decoding_message(QString& message){
        QStringList* decoded_message;
        for (int i=1; i<message.size();i++){
            int length=message.at(i).digitValue();
            decoded_message->push_back(message.mid(i+1, i+length));
        }
        return decoded_message;
}

QStringList* Entry_query::process_request(QString& message){
        Database_connection connection(message.at(0));
        if(connection.open_db_connection()){
            QSqlQuery query;
            QStringList* decoded_message=Template_query::decoding_message(message);
            QStringList* query_result;
            query.exec("SELECT name FROM users WHERE email= decoded_message.at(0) AND password=decoded-message.at(1)");
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
            query.exec("DELETE FROM films WHERE = decoded_message.at(0) AND password=decoded-message.at(1)");///делать тут
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
            query.exec("SELECT title, director, genre, year, rating, status"
                        "FROM films WHERE email= decoded_message.at(0)");
            while(query.next()){
                query_result->push_back(query.value(0).toString());
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
            query_existance.exec("SELECT name FROM users WHERE email= decoded_message.at(1) AND password=decoded-message.at(2)");
            if (query_existance.next()){
                query_result->push_back("0");
            }
            else{
                query.exec("INSERT INTO users (user_id, name, email, password)"
                           "VALUES (decoded_message.at(0), decoded_message.at(1), decoded_message.at(2))");
                while(query.next()){
                    query_result->push_back(query.value(0).toString());
                }
            }
            connection.close_db_connection();
            return query_result;
        }
        else return nullptr;
}
