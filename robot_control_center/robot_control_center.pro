#-------------------------------------------------
#
# Project created by QtCreator 2024-02-23T02:25:43
#
#-------------------------------------------------

QT       += core gui network
QT       += webengine webenginewidgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = robot_control_center
TEMPLATE = app
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS       += -lVLCQtCore -lVLCQtWidgets

SOURCES += \
        main.cpp \
        robotcontrol.cpp \
    robotcontroldashboard.cpp \
    jsonrobot.cpp \
    jsoncamera.cpp

HEADERS += \
        robotcontrol.h \
    robotcontroldashboard.h \
    jsonrobot.h \
    jsoncamera.h

FORMS += \
        robotcontrol.ui \
    robotcontroldashboard.ui

DISTFILES += \
    README.md \
    robots.json
