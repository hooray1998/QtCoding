#-------------------------------------------------
#
# Project created by QtCreator 2018-03-15T13:01:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo_1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mylabel.cpp \
    helpdoc.cpp \
    secwidget.cpp \
    mythread.cpp

HEADERS  += mainwindow.h \
    mylabel.h \
    helpdoc.h \
    secwidget.h \
    mythread.h

FORMS    += mainwindow.ui \
    helpdoc.ui \
    secwidget.ui

RESOURCES += \
    cpn.qrc \
    other/main.qrc \
    other/qss.qrc
RC_FILE = myapp.rc




