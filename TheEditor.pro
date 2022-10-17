QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Manager/ClientManager.cpp \
    Manager/OrderManager.cpp \
    Manager/ProductManager.cpp \
    Network/chattingclient.cpp \
    Network/server.cpp \
    View/addview.cpp \
    View/showview.cpp \
    View/view.cpp \
    main.cpp \
    mainwindow.cpp \
    tabwidget.cpp \
    tree.cpp \
    ttest.cpp

HEADERS += \
    Manager/ClientManager.h \
    Manager/OrderManager.h \
    Manager/ProductManager.h \
    Network/chattingclient.h \
    Network/server.h \
    View/addview.h \
    View/showview.h \
    View/view.h \
    mainwindow.h \
    tabwidget.h \
    tree.h \
    ttest.h

FORMS += \
    Network/addParticipantView.ui \
    server.ui \
    addClient.ui \
    addOrder.ui \
    addProduct.ui \
    mainwindow.ui \
    showClient.ui \
    showOrder.ui \
    ttest.ui

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
