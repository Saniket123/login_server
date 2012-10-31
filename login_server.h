#ifndef LOGIN_SERVER_H
#define LOGIN_SERVER_H

#include <QObject>
#include <QString>
#include <QUuid>
#include "tcp_receiver.h"

class login_server : public QObject
{
    Q_OBJECT
public:
    explicit login_server(QObject *parent = 0);
    ~login_server();
    void start();
private:
    tcp_receiver *_tcp_receiver;
    QString *url;
    QString s_code;
private slots:
    int recognize(QString file_name);
    void send_s_code(bool is_loggedin);
};

#endif // LOGIN_SERVER_H
