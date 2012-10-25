#include <QCoreApplication>
#include "login_server.h"

login_server::login_server(QObject *parent) :
    QObject(parent)
{
    this->_tcp_receiver = new tcp_receiver;

    connect(this->_tcp_receiver, SIGNAL(img_received(QString)), this, SLOT(recognize(QString)));
}
login_server::~login_server()
{
    delete this->_tcp_receiver;
}
void login_server::start()
{
    this->_tcp_receiver->start();
}
int login_server::recognize(QString file_name)
{
    qDebug() << "img recevide ok!";
    qDebug() << "img path:" << QCoreApplication::applicationDirPath() + '/' + file_name;
    return 0;
}
