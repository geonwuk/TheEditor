#include "message.h"
#include <QDebug>
#include <QIODevice>
#include <iostream>
Message::Message(QString str, REQUEST req): type(req){
   data.append(str.toUtf8());
}

Message::Message(QString str, RESPOND req): type(req){
   data.append(str.toUtf8());
}



Message::operator const QByteArray() const{
   QByteArray re;
   QDataStream stream (&re, QIODevice::WriteOnly);
   quint64 data_size = sizeof(type)+data.size();
   stream<<data_size;


   qDebug()<<"send"<<re<<","<<re.size();
   stream<<type;
   qDebug()<<"send"<<re<<","<<re.size();
   for(auto e : data){
       stream<<e;
   }

//   stream<<data;
   qDebug()<<"send"<<re<<","<<re.size();
   return re;
}
