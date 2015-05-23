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
    filetreemodel.cpp \
    imagemetadata.cpp \
    audiometadata.cpp

HEADERS  += mainwindow.h \
    bookmarklistwidgetitem.h \
    filetreeitem.h \
    filetreemodel.h \
    imagemetadata.h \
    audiometadata.h

FORMS    += mainwindow.ui

RESOURCES     = medium.qrc

ICON = ./icons/medium-icon.icns

unix:!macx: LIBS += -L$$PWD/exiv2/lib/ -lexiv2

INCLUDEPATH += $$PWD/exiv2/include
DEPENDPATH += $$PWD/exiv2/include

unix|win32: LIBS += -ltag
