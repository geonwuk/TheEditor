QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DB/DB_ClientManager.cpp \
    DB/DB_Manager.cpp \
    DB/DB_OrderManager.cpp \
    DB/DB_ProductManager.cpp \
    MainManager.cpp \
    Manager/ClientManager.cpp \
    Manager/OrderManager.cpp \
    Manager/ProductManager.cpp \
    Model/Model.cpp \
    Model/Type.cpp \
    Network/logmanagement.cpp \
    Network/logthread.cpp \
    Network/message.cpp \
    Network/server.cpp \
    Network/servermanager.cpp \
    Tree.cpp \
    View/AddView.cpp \
    View/GeneralView.cpp \
    View/ViewFactory.cpp \
    View/showview.cpp \
    main.cpp \
    mainwindow.cpp \
    tabwidget.cpp

HEADERS += \
    DB/DB_ClientManager.h \
    DB/DB_Manager.h \
    DB/DB_OrderManager.h \
    DB/DB_ProductManager.h \
    MainManager.h \
    Manager/ClientManager.h \
    Manager/OrderManager.h \
    Manager/ProductManager.h \
    Model/Model.h \
    Model/Type.h \
    Network/logmanagement.h \
    Network/logthread.h \
    Network/message.h \
    Network/server.h \
    Network/servermanager.h \
    Tree.h \
    View/AddView.h \
    View/GeneralView.h \
    View/ViewFactory.h \
    View/showview.h \
    mainwindow.h \
    tabwidget.h

FORMS += \
    Network/addParticipantView.ui \
    Network/chatRoom.ui \
    addClient.ui \
    addOrder.ui \
    addProduct.ui \
    dashboard.ui \
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
    resources.qrc
