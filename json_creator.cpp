#include "json_creator.h"
#include <QString>

Json_creator::Json_creator(QString request_code, bool error_code, QString email){
    json_data["RequestCode"] = request_code;
    json_data["ErrorCode"]   = Json_creator::bool_to_str(error_code);
    json_data["Email"]       = email;
}

Json_creator::Json_creator(QString request_code, bool error_code, QString email, QString name){
    json_data["RequestCode"] = request_code;
    json_data["ErrorCode"]   = Json_creator::bool_to_str(error_code);
    json_data["Email"]       = email;
    json_data["Name"]        = name;
}

Json_creator::Json_creator(QString request_code, bool error_code, QString email, QJsonArray rows){
    json_data["RequestCode"]= request_code;
    json_data["ErrorCode"]  = Json_creator::bool_to_str(error_code);
    json_data["Email"]      = email;
    json_data["Rows"]       = rows;
}

Json_creator::Json_creator(QString request_code, bool error_code, QString email, QJsonObject row_new, QJsonObject row_old){
    json_data["RequestCode"] = request_code;
    json_data["ErrorCode"]   = Json_creator::bool_to_str(error_code);
    json_data["Email"]       = email;
    json_data["Row_new"]     = row_new;
    json_data["Row_old"]     = row_old;
}

Json_creator::Json_creator(){
}

Json_creator::~Json_creator(){
    qDebug() << "in ~Json_creator";
}

QJsonObject Json_creator::get_json_data(){
    return this->json_data;
}

QString Json_creator::bool_to_str(bool status){
    return status? "1":"0";
}

void Json_creator::adding_information(QString key, QString value){
    json_data.insert(key, value);
}

void Json_creator::changing_error_code(bool error_code){
    QJsonObject::iterator iter = json_data.find("ErrorCode");
    *iter = error_code;
}

