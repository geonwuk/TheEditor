#include "message.h"
#include <QDebug>
#include <QIODevice>
#include <iostream>
#include <QDataStream>
#include <QFile>
#include <QProgressDialog>
Message::Message(QString str, REQUEST req): type(req) {
   data.append(str.toUtf8());
}

Message::operator const QByteArray() const{
   QByteArray re;
   QDataStream stream (&re, QIODevice::WriteOnly);
   quint64 data_size = sizeof(type)+data.size();
   stream<<data_size;
   stream<<type;
   for(auto e : data){
       stream<<e;
   }
//   stream<<data;
   qDebug()<<"send"<<re<<","<<re.size();
   return re;
}

ReadMessage::ReadMessage(QByteArray& data) : data{data} {
    QDataStream in{&data, QIODevice::ReadOnly};
    in.readRawData(&type,1);
}


ReadMessage::operator const char() const{
    return type;
}

const QString ReadMessage::toQString() const {
    QString result {data};
    result.remove(0,1);
    qDebug()<<"to QString"<<result;
    return result;
}


FileMessage::FileMessage(QFile* file, QProgressDialog* progress_dialog):file{file},progress_dialog{progress_dialog}{
    qint64 bytes_read=0;
    file_size = file->size();
    if(file_size==0)
        throw -1;

    QDataStream stream (&data, QIODevice::WriteOnly);
    quint64 data_size = sizeof(char)+file_size;
    stream<<data_size;
    stream<<static_cast<char>(Chat_FileTransmission);
    while(bytes_read<file_size){
        auto data_read = file->read(file_size);
        data.append(data_read);
        if(data_read.size()==0){
            throw -1;
        }
        bytes_read += data_read.size();
        progress_dialog->setValue(bytes_read);
    }
}

FileMessage::operator const QByteArray() const{
   qDebug()<<"sending data to network of which size is"<<data.size();
   return data;
}
