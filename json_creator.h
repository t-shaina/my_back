#ifndef JSON_CREATOR_H
#define JSON_CREATOR_H


#include<QStringList>
#include<QJsonObject>
#include<QJsonArray>
#include<QString>

class Json_creator
{

public:
    Json_creator(QString request_code, bool error_code, QString email);
    Json_creator(QString request_code, bool error_code, QString email, QString name);
    Json_creator(QString request_code, bool error_code, QString email, QJsonArray rows);
    Json_creator(QString request_code, bool error_code, QString email, QJsonObject row_new, QJsonObject row_old);
    Json_creator();
    QJsonObject get_json_data();
    void adding_information(QString key, QString value);
    void changing_error_code(bool error_code);
    ~Json_creator();
private:
    QJsonObject json_data;
};

#endif // JSON_CREATOR_H
