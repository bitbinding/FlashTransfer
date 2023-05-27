#-------------------------------------------------
#
# Project created by QtCreator 2015-08-21T11:40:12
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FlashTransfer
TEMPLATE = app

INCLUDEPATH += "E:/Qt/Qt5.7.0/Tools/mingw530_32/opt/include"
LIBS += "E:/Qt/Qt5.7.0/Tools/mingw530_32/opt/lib/libregex.a"

PRECOMPILED_HEADER = stable.h

RC_FILE = FlashTransfer.rc

SOURCES += main.cpp\
        mainwindow.cpp\
    confusesetting.cpp\
    mainprogress.cpp

HEADERS  += mainwindow.h\
    confusesetting.h\
    mainprogress.h\
    transferthreadclass.h

FORMS    += mainwindow.ui\
    confusesetting.ui\
    mainprogress.ui

DISTFILES += \
    FlashTransfer.rc
