#include "message.h"
#include <QDebug>
#include <QIODevice>
#include <iostream>
#include <QDataStream>

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


