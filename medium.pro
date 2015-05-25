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
    imagemetadata.cpp \
    audiometadata.cpp\
    pdfmetadata.cpp \
    filetreewidgetitem.cpp \
    fileindexer.cpp

HEADERS  += mainwindow.h \
    bookmarklistwidgetitem.h \
    imagemetadata.h \
    audiometadata.h\
    pdfmetadata.h \
    filetreewidgetitem.h \
    fileindexer.h

FORMS    += mainwindow.ui

RESOURCES     = medium.qrc

ICON = ./icons/medium-icon.icns

unix:!macx: LIBS += -L$$PWD/exiv2/lib/ -lexiv2

INCLUDEPATH += $$PWD/exiv2/include
DEPENDPATH += $$PWD/exiv2/include

unix|win32: LIBS += -ltag

unix: LIBS += -L$$PWD/taglib/lib/ -ltag

INCLUDEPATH += $$PWD/taglib/include
DEPENDPATH += $$PWD/taglib/include

unix:!macx: LIBS += -L$$PWD/PoDoFo/lib/ -lpodofo
unix:!macx:LIBS += -lz
unix:!macx:LIBS += -lfreetype -ljpeg -lfontconfig

INCLUDEPATH += $$PWD/PoDoFo/include
DEPENDPATH += $$PWD/PoDoFo/include

unix:!macx: PRE_TARGETDEPS += $$PWD/PoDoFo/lib/libpodofo.a
