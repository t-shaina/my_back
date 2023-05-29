#include"socket.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Socket db_server(&a);
    return a.exec();
}
