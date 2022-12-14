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

    return a.exec();
}
