#ifndef TEMPLATE_QUERY_H
#define TEMPLATE_QUERY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include<QChar>

enum Query_id{entry_id=0, delete_id=1, select_id=2, update_id=3, registration_id=4, select_all_id=5};

class Template_query: public QObject
{
    Q_OBJECT
public:
    Template_query();
    virtual ~Template_query();
    static Template_query* create_template_query(Query_id  id);
    static QStringList* decoding_message(const QString& message);
    static QString* encoding_message(const QStringList* data_list);
    static int* get_user_id(const QString& email);
    static QList<int>* select_all_for_user(const int* user_id);
    static QStringList* select_all_for_record(const int* user_id, const int* film_id, const QChar type_connection='8');
    static QStringList*  exist_query(const QStringList*  data_list, const QChar type_connection='9');
    virtual QStringList* process_request(QString &message)=0;

};

class Entry_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
};

class Delete_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
};

class Select_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
};

class Update_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
    static bool directors_update(QStringList* directors, int gotten_film_id, const QChar type_connection);
    static bool genres_update(QStringList* directors, int gotten_film_id, const QChar type_connection);

};

class Registration_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
};
class Select_all_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
};
#endif // TEMPLATE_QUERY_H
