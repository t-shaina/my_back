#include "socket_adapter.h"
#include<QByteArray>
#include<QDataStream>
ISocket_adapter::ISocket_adapter(QObject* parent)
    : QObject(parent)
{}
ISocket_adapter:: ~ISocket_adapter(){}


Socket_adapter::Socket_adapter(QObject* parent, QTcpSocket* p_socket)
    :ISocket_adapter(parent), block_size_in(0)
{
    qDebug()<< "new Socket_adapter";
    if(p_socket==0){
        socket_=new QTcpSocket(this);
    }
    else socket_=p_socket;
    connect(socket_, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    connect(socket_, SIGNAL(disconnected()), this, SLOT(on_disconnected()));

}
Socket_adapter:: ~Socket_adapter(){}
void Socket_adapter::on_readyRead(){
    qDebug()<< "in readyRead";
    QByteArray buf;
    QDataStream in(socket_);

    while(true){
        qDebug()<<"block_size_in="<< block_size_in;
        if (block_size_in==0){
            if(socket_->bytesAvailable()<sizeof(int))
                return;
            in>>block_size_in;

            //qDebug()<<"block_size_in="<< block_size_in;
        }
        else{
            if(socket_->bytesAvailable()<block_size_in)
            return;
            in>>buf;
            block_size_in=0;
            //qDebug() <<buf;
            emit have_new_message(buf);
        }
    }
}
void Socket_adapter::on_disconnected(){
    qDebug()<<"in disconnected in Socket_adapter";
    socket_->deleteLater();
    emit disconnected();
}
void Socket_adapter::sendData(const QByteArray& message){
    qDebug()<<"in senddata"<< message;
    QByteArray block;
    QDataStream sendStream(&block, QIODevice::ReadWrite);
    sendStream << quint16(0) << message;
    sendStream.device()->seek(0);
    sendStream << (quint16)(block.size() - sizeof(quint16));
    socket_->write(block);
}
