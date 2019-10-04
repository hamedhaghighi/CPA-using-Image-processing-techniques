#-------------------------------------------------
#
# Project created by QtCreator 2017-08-25T19:56:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencv_training
TEMPLATE = app
INCLUDEPATH+=E:\opencv\build\include
LIBS+= "E:\opencv\build\x86\vc11\dlib\*.lib"
SOURCES += main.cpp\
        mainwindow.cpp \
    Contours.cpp \
    extra.cpp \
    preprocessing.cpp \
    registration.cpp \
    initialization.cpp
HEADERS  += mainwindow.h \
    Contours.h \
    extra.h \
    preprocessing.h \
    registrarion.h

FORMS    += mainwindow.ui
