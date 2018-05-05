#-------------------------------------------------
#
# Project created by QtCreator 2017-11-04T11:00:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AmbiMqttNodeMcu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the0
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -DLinuxX11
LIBS += -lX11 -lmosquittopp

INCLUDEPATH += src include

SOURCES += \
    src/draw_grid.cpp \
    src/gui.cpp \
    src/main.cpp \
    src/MqttClient.cpp \
    src/terminal.cpp \
    src/settings.cpp

HEADERS += \
    include/config.h \
    include/draw_grid.h \
    include/gui.h \
    include/MqttClient.h \
    include/terminal.h \
    include/settings.h

DISTFILES += \
    icon/colored.svg \
    icon/monochrome.svg

include(../../Qt-Color-Widgets/color_widgets.pri)

FORMS += \
    settingswindow.ui
