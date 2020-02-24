#-------------------------------------------------
#
# Project created by QtCreator 2020-02-05T18:10:30
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MQTT_Smart_Home
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += $$PWD/sdk

CONFIG(debug, debug|release){
LIBS += -L$$PWD/sdk/ -lqucd
} else {
LIBS += -L$$PWD/sdk/ -lquc
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQt5Qmqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQt5Qmqttd
else:unix: LIBS += -L$$PWD/lib/ -lQt5Qmqtt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQt5Qmqtt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQt5Qmqttd
else:unix: LIBS += -L$$PWD/lib/ -lQt5Qmqtt

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
