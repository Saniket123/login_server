#include "tcp_receiver.h"

tcp_receiver::tcp_receiver()
{
    this->total_bytes = this->bytes_received = this->file_name_size = 0;
    connect(&this->tcp_server, SIGNAL(newConnection()), this, SLOT(accept_connection()));
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
    this->tcp_server_connection = this->tcp_server.nextPendingConnection();
    connect(this->tcp_server_connection, SIGNAL(readyRead()), this, SLOT(receive()));
    connect(this->tcp_server_connection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(display_error(QAbstractSocket::SocketError)));
    this->tcp_server.close();
}

void tcp_receiver::display_error(QAbstractSocket::SocketError socket_error)
{
    qDebug() << "ooooooooooooooooooooooooo" << tcp_server_connection->errorString();
    tcp_server_connection->close();
}

void tcp_receiver::receive()
{
    qDebug() << "in receive";

    QDataStream in(this->tcp_server_connection);
    if(this->bytes_received <= sizeof(qint64) * 2) { //如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
        if((this->tcp_server_connection->bytesAvailable() >= sizeof(qint64) * 2) && (this->file_name_size == 0)) { //接收数据总大小信息和文件名大小信息
            in >> this->total_bytes >> this->file_name_size;
            qDebug() << this->total_bytes;
            qDebug() << this->file_name_size;
            this->bytes_received += sizeof(qint64) * 2;
        }
        if((this->tcp_server_connection->bytesAvailable() >= this->file_name_size) && (this->file_name_size != 0)) {  //接收文件名，并建立文件
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
       this->bytes_received += this->tcp_server_connection->bytesAvailable();
       this->in_block = this->tcp_server_connection->readAll();
       this->local_file->write(this->in_block);
       this->in_block.resize(0);
   }
   if(this->bytes_received == this->total_bytes) { //接收数据完成时
    this->tcp_server_connection->close();
    this->local_file->close();
    emit img_received(this->local_file->fileName());
   }
}
