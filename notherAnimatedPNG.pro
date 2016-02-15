#-------------------------------------------------
#
# Project created by QtCreator 2016-02-14T19:57:17
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = notherAnimatedPNG
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    diffimage.cpp \
    lsbhider.cpp

HEADERS  += mainwindow.h \
    diffimage.h \
    imageoperations.h \
    lsbhider.h

FORMS    += mainwindow.ui
