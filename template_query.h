#ifndef TEMPLATE_QUERY_H
#define TEMPLATE_QUERY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include<QChar>
#include<QJsonObject>
#include"json_creator.h"
enum Query_id{entry_id=0, delete_id=1, select_id=2, insert_id=3, registration_id=4, select_all_id=5, update_id=6};

class Template_query: public QObject
{
    Q_OBJECT
public:
    Template_query();
    virtual ~Template_query();
    static Template_query* create_template_query(Query_id  id);
    static QStringList decoding_json_object(const QJsonValue& object);
    //static QString* encoding_message(const QStringList* data_list);
    //static int number_of_tens(int size);
    static int get_user_id(const QString& email, const QString parent_type_connection);
    static int get_film_id(const int* user_id, const QJsonObject &object, const QString parent_type_connection);
    static void select_all_for_user(QList<int>* films_id, const int* user_id, const QString parent_type_connection);
    static QJsonObject select_all_for_record(const int* user_id, const int* film_id, const QString parent_type_connection);
    static bool exist_query(const QJsonObject& object, const QString parent_type_connection);
    virtual  Json_creator process_request(QJsonObject &object)=0;

};

class Entry_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
};

class Delete_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
    static bool directors_delete(QStringList* directors, int* gotten_film_id, const QString parent_type_connection);
    static bool genres_delete(QStringList* genres, int* gotten_film_id, const QString parent_type_connection);
};

class Select_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
};

class Insert_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
    static bool directors_insert(QStringList* directors, int* gotten_film_id, const QString parent_type_connection);
    static bool genres_insert(QStringList* genres, int* gotten_film_id, const QString parent_type_connection);

};

class Registration_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
};
class Select_all_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
};
class Update_query: public Template_query{
    Q_OBJECT
public:
    Json_creator process_request(QJsonObject &object);
    static bool directors_update(QStringList* directors, int* gotten_film_id, const QString parent_type_connection);
    static bool genres_update(QStringList* genres, int* gotten_film_id, const QString parent_type_connection);
};
#endif // TEMPLATE_QUERY_H
