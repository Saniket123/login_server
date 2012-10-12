#include <QCoreApplication>
#include "tcp_receiver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    tcp_receiver my_tcp_receiver;
    my_tcp_receiver.start();
    return a.exec();
}
