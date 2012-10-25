#include <QCoreApplication>
#include "login_server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    login_server my_login_server;
    my_login_server.start();
    return a.exec();
}
