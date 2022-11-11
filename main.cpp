#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "DB/dbmanager.h"

extern const char name[] = "Client";
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("Fusion");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TheEditor_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();


//    DBManager<name> db;
//    auto x = db.add("Kag1","Kag2","Kag3","Kag4");
//    x.exec();
//    qDebug()<<x.lastError();

//    auto y = db.modify("Kag1","Kag1","a","b","c");
//    y.exec();
//    qDebug()<<y.lastError();

    return a.exec();
}
