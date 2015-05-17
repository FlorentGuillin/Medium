#-------------------------------------------------
#
# Project created by QtCreator 2015-04-17T10:41:52
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = medium
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bookmarklistwidgetitem.cpp \
    filetreeitem.cpp \
    filetreemodel.cpp

HEADERS  += mainwindow.h \
    bookmarklistwidgetitem.h \
    filetreeitem.h \
    filetreemodel.h

FORMS    += mainwindow.ui

RESOURCES     = medium.qrc

ICON = ./icons/medium-icon.icns
