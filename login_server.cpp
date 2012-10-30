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
void login_server::send_url()
{
    this->_tcp_receiver->connection_socket->write(url->toAscii(), url->length());
}
int login_server::recognize(QString file_name)
{
    int user_id = -1;
    qDebug() << "in recognize";
    IplImage *_img;
    _img = cvLoadImage(file_name.toLatin1().data(), 0);//灰度图
    face_recognition fr;
    fr.init_func((unsigned char *)_img->imageDataOrigin);
    fr.get_face_parameters();
    fr.get_face_img();
    int i;
    for (i = 0; i < 100; i ++) {
        if (fr._face_recognition(i, 10.0) == 1)
            break;
    }
    if (i < 100) user_id = i;

    fr.release_func();
    cvReleaseImage(&_img);

    url->append(QString::number(user_id));
    send_url();
    return 0;
}
