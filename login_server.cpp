#include <QCoreApplication>
#include "login_server.h"
#include "face_recognition.h"

login_server::login_server(QObject *parent) :
    QObject(parent)
{
    this->url = new QString("http://www.baidu.com/?id=");
    this->_tcp_receiver = new tcp_receiver;

    connect(this->_tcp_receiver, SIGNAL(img_received(QString)), this, SLOT(recognize(QString)));    
}
login_server::~login_server()
{
    delete this->url;
    delete this->_tcp_receiver;
}
void login_server::start()
{
    this->_tcp_receiver->start();
}
void login_server::send_s_code(bool is_loggedin)
{
    qDebug() << "sending s code";
    if (is_loggedin) {
        this->s_code = (QUuid::createUuid()).toString();
    } else {
        this->s_code = "false";
    }
    this->_tcp_receiver->connection_socket->write(s_code.toAscii(), s_code.length());
}
int login_server::recognize(QString file_name)
{
    bool status;
    int user_id = this->_tcp_receiver->username.toInt();
    qDebug() << "in recognize";
    IplImage *_img;
    _img = cvLoadImage(file_name.toLatin1().data(), 0);//灰度图
    face_recognition fr;
    fr.init_func(_img);
    fr.get_face_parameters();
    fr.get_face_img();
    if (fr._face_recognition(user_id, 0.93) == 1) status = true;
    else status = false;

    fr.release_func();
    cvReleaseImage(&_img);

    send_s_code(status);
    return 0;
}
