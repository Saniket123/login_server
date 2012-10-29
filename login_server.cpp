#include <QCoreApplication>
#include "login_server.h"
#include "face_recognition.h"

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
    qDebug() << "in recognize";
    IplImage *face_img;
    face_img = cvLoadImage(file_name.toLatin1().data(), 0);//灰度图

    face_recognition fr;
    fr.init_func((unsigned char *)face_img->imageData);
    fr.get_face_parameters();
    fr.get_face_img();
    int rst = fr._face_recognition(5, 10.0);

    if (rst == 0) qDebug() << "rst   0";
    else if (rst == 1) qDebug() << "rst   1";
    else qDebug() << "rst   -1";


    fr.release_func();
    cvReleaseImage(&face_img);
    qDebug() << "img recevide ok!";
    qDebug() << "img path:" << QCoreApplication::applicationDirPath() + '/' + file_name;
    return 0;
}
