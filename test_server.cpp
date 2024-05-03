#include "test_server.h"
#include "json_creator.h"
#include "template_query.h"

#include <QByteArray>
#include <QVariantMap>
#include <QDataStream>
#include <QTcpSocket>
#include <QJsonParseError>
#include <QJsonDocument>

Test_server::Test_server(QObject *parent)
    : QObject(parent){
    server_ = new QTcpServer(this);
    connect(server_, SIGNAL(newConnection()), this, SLOT(on_newConnection()));
    m_clients = new QList<ISocket_adapter*>();
    m_clients->reserve(5);

    if (!server_->listen(QHostAddress::Any, 1234)){
        qDebug() << "Server could not start";
    } else {
        qDebug() << "Server started";
    }
}

Test_server::~Test_server(){
    delete m_clients;
    delete server_;
}

void Test_server::on_newConnection(){
    qDebug() << "in new connection";

    QTcpSocket* clientConnection = server_->nextPendingConnection();
    ISocket_adapter* socket_handle = new Server_socket_adapter(clientConnection);
    m_clients->push_back(socket_handle);
    connect(socket_handle, SIGNAL(disconnected()), SLOT(on_disconnected()));
    connect(socket_handle, SIGNAL(have_new_message(QByteArray)), SLOT(on_message(QByteArray)));
}

void Test_server::on_disconnected(){
    qDebug() << "client disconnected";
    ISocket_adapter* client = static_cast<Server_socket_adapter*>(sender());
    m_clients->removeOne(client);
}

void Test_server::on_message(QByteArray message){
    qDebug() << "in on_message";
    ISocket_adapter* client = static_cast<Server_socket_adapter*>(sender());
    QJsonParseError parse_error;
    QJsonDocument json_doc;
    QJsonObject object;
    json_doc = QJsonDocument::fromJson(message, &parse_error);
    if (parse_error.error != QJsonParseError::NoError) {
        qWarning() << "Parse error at" << parse_error.offset << ":" << parse_error.errorString();
    }
    else {
        object = json_doc.object();
    }

    QString request_code = object.value("RequestCode").toString();

    Query_id query_id = static_cast<Query_id>(request_code.toInt());
    Template_query* query_type = Template_query::create_template_query(query_id);
    Json_creator result = query_type->process_request(object);
    client->sendData(QJsonDocument(result.get_json_data()).toJson());
}

Server_socket_adapter::Server_socket_adapter(QTcpSocket* p_socket, QObject* parent)
    : Socket_adapter(parent, p_socket){
}
