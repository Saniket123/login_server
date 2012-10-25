#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

#include <QtNetwork/QtNetwork>
#include <QDebug>

class tcp_receiver : public QObject
{
    Q_OBJECT
public:
    tcp_receiver();
private:
    QTcpServer tcp_server;
    QTcpSocket *connection_socket;
    qint64 total_bytes;
    qint64 bytes_received;
    qint64 file_name_size;
    QString file_path;
    QFile *local_file;
    QByteArray in_block;
private slots:
    void accept_connection();
    void receive();
    void display_error(QAbstractSocket::SocketError socket_error);
    void send_url(QString url);
    void clean_up();
signals:
    void img_received(QString file_path);
public:
    void start();
};

#endif // TCP_RECEIVER_H
