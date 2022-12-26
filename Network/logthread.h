#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>
#include <QList>

class QTreeWidgetItem;

class LogThread : public QThread
{
    Q_OBJECT
public:
    explicit LogThread(QObject *parent = nullptr);

private:
    void run() override;
    QList<QTreeWidgetItem*> itemList;               //QTreeWidget에서 데이터 추출
    std::string filename;                               //저장할 파일 이름

signals:
    void send(int data);

public slots:
    void appendData(QTreeWidgetItem*);
    void saveData();
};

#endif // LOGTHREAD_H
