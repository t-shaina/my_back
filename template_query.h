#ifndef TEMPLATE_QUERY_H
#define TEMPLATE_QUERY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include<QChar>

enum Query_id{entry_id=0, delete_id=1, select_id=2, update_id=3, registration_id=4};

class Template_query: public QObject
{
    Q_OBJECT
public:
    Template_query();
    virtual ~Template_query();
    static Template_query* create_template_query(Query_id  id);
    static QStringList* decoding_message(const QString& message);
    static int* get_user_id(const QString& email);
    static QList<int>* select_all_for_user(const int* user_id);
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
};

class Registration_query: public Template_query{
    Q_OBJECT
public:
    QStringList* process_request(QString &message);
};
#endif // TEMPLATE_QUERY_H
