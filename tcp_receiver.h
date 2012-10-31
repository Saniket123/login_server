#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H

#include <QString>
#include <QtNetwork/QtNetwork>
#include <QDebug>

class tcp_receiver : public QObject
{
    Q_OBJECT
public:
    tcp_receiver();
    QTcpSocket *connection_socket;
    QString username;

private:
    QTcpServer tcp_server;
    qint64 total_bytes;
    qint64 bytes_received;
    qint64 file_name_size;
    QString file_path;
    QFile *local_file;
    QByteArray in_block;

    void send_confirm_username();
private slots:
    void accept_connection();
    void receive();
    void display_error(QAbstractSocket::SocketError socket_error);
    void clean_up();
    void receive_username();

signals:
    void img_received(QString file_path);
public:
    void start();
};

#endif // TCP_RECEIVER_H
