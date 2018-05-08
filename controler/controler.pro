#-------------------------------------------------
#
# Project created by QtCreator 2018-05-08T19:18:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = controler
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

QMAKE_CXXFLAGS += -DLinuxX11
LIBS += -lX11 -lmosquittopp

SOURCES += \
    src/draw_grid.cpp \
    src/gui.cpp \
    src/main.cpp \
    src/MqttClient.cpp \
    src/settings.cpp \
    src/terminal.cpp

HEADERS += \
    inc/config.h \
    inc/draw_grid.h \
    inc/gui.h \
    inc/MqttClient.h \
    inc/settings.h \
    inc/terminal.h

FORMS += \
    settingswindow.ui

DISTFILES += \
    ico/colored.svg \
    ico/monochrome.svg

include(dep/Qt-Color-Widgets/color_widgets.pri)
