#include "message.h"
#include <QDebug>
#include <QIODevice>
#include <iostream>
#include <QDataStream>
#include <QFile>
#include <QProgressDialog>
#include <QFileInfo>
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
   return re;
}

ReadMessage::ReadMessage(QByteArray& data) : data{data} {
    QDataStream in{&data, QIODevice::ReadOnly};
    in.readRawData(&type,1);
    data.remove(0,1);
}


ReadMessage::operator const char() const{
    return type;
}

const QString ReadMessage::toQString() const {
    QString result;
    result= QString::fromUtf8(data);
    return result;
}


FileMessage::FileMessage(QFile* file, QProgressDialog* progress_dialog, QString sender_name):sender_name{sender_name},file{file},progress_dialog{progress_dialog}{
    qint64 bytes_read=0;
    file_size = file->size();
    if(file_size==0)
        throw -1;

    QDataStream stream (&data, QIODevice::WriteOnly);
    QFileInfo file_info{file->fileName()};
    QByteArray file_name = file_info.fileName().toUtf8();
    QByteArray sender_name_ = sender_name.toUtf8();

    quint64 data_size = sizeof(char)+file_size+file_name.size()+sender_name_.size()+2;
    stream<<data_size;
    stream<<static_cast<char>(Chat_FileTransmission);
    for(auto e : file_name){                        //파일 이름 추가
        stream<<e;
    }
    stream<<'\0';                                   //null문자 삽입
    for(auto e : sender_name_){                     //보낸 ID 추가
        stream<<e;
    }
    stream<<'\0';                                   //null문자 삽입

    while(bytes_read<file_size){
        progress_dialog->setValue(10);              //progress_dialog의 50%는 파일을 읽을 떄, 나머지 50%는 서버에 업로드할 때 갱신
        auto data_read = file->read(file_size);     //Todo: 파일이 읽힌느 도중에 progress_dialog가 에상 외로 바뀌지 않음 (수정 필요)
        data.append(data_read);
        if(data_read.size()==0){                    //파일 읽기 중 오류가 나면 예외 처리
            throw -1;
        }
        bytes_read += data_read.size();             //읽으 데이터 값에 추가
        progress_dialog->setValue(bytes_read);
    }
}

FileMessage::operator const QByteArray() const{
   return data;
}
void FileMessage::updateProgress(qint64 bytes_written){
    auto value = progress_dialog->maximum()/2+bytes_written;                         //progress_dialog의 50%는 파일을 읽을 떄, 나머지 50%는 서버에 업로드할 때 갱신
    value >= progress_dialog->maximum() ? progress_dialog->maximum() : value;
    progress_dialog->setValue(value);                                                 //서버로 부터 업로드된 데이터 값으로 업데이트
}
void FileMessage::completeProgress(){
    progress_dialog->setValue( progress_dialog->maximum());
}
