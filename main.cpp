//#include"socket.h"
#include <QCoreApplication>
#include "test_server.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Test_server* server=new Test_server(&a);
    //Socket db_server(&a);
    return a.exec();
}
