QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DB/dbmanager.cpp \
    Manager/ClientManager.cpp \
    Manager/OrderManager.cpp \
    Manager/ProductManager.cpp \
    Model/model.cpp \
    Model/type.cpp \
    Network/logthread.cpp \
    Network/message.cpp \
    Network/server.cpp \
    Network/servermanager.cpp \
    View/addview.cpp \
    View/showview.cpp \
    View/view.cpp \
    main.cpp \
    mainwindow.cpp \
    tabwidget.cpp \
    tree.cpp

HEADERS += \
    DB/dbmanager.h \
    Manager/ClientManager.h \
    Manager/OrderManager.h \
    Manager/ProductManager.h \
    Model/model.h \
    Model/type.h \
    Network/logthread.h \
    Network/message.h \
    Network/server.h \
    Network/servermanager.h \
    View/addview.h \
    View/showview.h \
    View/view.h \
    mainwindow.h \
    tabwidget.h \
    tree.h

FORMS += \
    Network/addParticipantView.ui \
    Network/chatRoom.ui \
    addClient.ui \
    addOrder.ui \
    addProduct.ui \
    mainwindow.ui \
    showClient.ui \
    showOrder.ui

TRANSLATIONS += \
    TheEditor_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    SQLITE_queries.qrc \
    resources.qrc
