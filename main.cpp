#include"data_base.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Data_base_server db_server(&a);
    return a.exec();
}
