#include "tcp_receiver.h"

tcp_receiver::tcp_receiver()
{
    this->total_bytes = this->bytes_received = this->file_name_size = 0;
    connect(&this->tcp_server, SIGNAL(newConnection()), this, SLOT(accept_connection()));
    connect(this, SIGNAL(img_received(QString)), this, SLOT(send_url(QString)));
}

void tcp_receiver::send_url(QString url)
{
    this->connection_socket->write(url.toAscii(), url.length());
}

void tcp_receiver::start()
{
    this->bytes_received = 0;
    if (!this->tcp_server.listen(QHostAddress::LocalHost, 8888)) {
        qDebug() << this->tcp_server.errorString();
        this->tcp_server.close();
        return;
    }
}

void tcp_receiver::accept_connection()
{
    this->connection_socket = this->tcp_server.nextPendingConnection();
    connect(this->connection_socket, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(this->connection_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(display_error(QAbstractSocket::SocketError)));
//    this->tcp_server.close();
}

void tcp_receiver::display_error(QAbstractSocket::SocketError socket_error)
{
    qDebug() << "ooooooooooooooooooooooooo" << connection_socket->errorString();
    connection_socket->close();
}

void tcp_receiver::clean_up()
{
    this->total_bytes = this->bytes_received = this->file_name_size = 0;
}

void tcp_receiver::receive()
{
    qDebug() << "in receive";

    QDataStream in(this->connection_socket);
    if(this->bytes_received <= sizeof(qint64) * 2) { //如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到来的头文件信息
        if((this->connection_socket->bytesAvailable() >= sizeof(qint64) * 2) && (this->file_name_size == 0)) { //接收数据总大小信息和文件名大小信息
            in >> this->total_bytes >> this->file_name_size;
            qDebug() << this->total_bytes;
            qDebug() << this->file_name_size;
            this->bytes_received += sizeof(qint64) * 2;
        }
        if((this->connection_socket->bytesAvailable() >= this->file_name_size) && (this->file_name_size != 0)) {  //接收文件名，并建立文件
            in >> this->file_path;
            qDebug() << this->file_path;
            this->bytes_received += this->file_name_size;
            this->local_file = new QFile(this->file_path);
            if(!this->local_file->open(QFile::WriteOnly)) {
                qDebug() << "open file error!";
                return;
            }
        } else return;
   }
   if(this->bytes_received < this->total_bytes) {  //如果接收的数据小于总数据，那么写入文件
       this->bytes_received += this->connection_socket->bytesAvailable();
       this->in_block = this->connection_socket->readAll();
       this->local_file->write(this->in_block);
       this->in_block.resize(0);
   }
   if(this->bytes_received == this->total_bytes) { //接收数据完成时
//    this->connection_socket->close();
    this->local_file->close();
       clean_up();
    emit img_received(this->local_file->fileName());
   }
}
