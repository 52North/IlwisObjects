#-------------------------------------------------
#
# Project created by QtCreator 2014-07-14T08:25:03
#
#-------------------------------------------------

QT       += core

QT       -= gui

include(global.pri)

TARGET = ilwisconsole
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += commandlineclient/main.cpp

INEXEPATH=$$LIBPATH/$$TARGET
OUTEXEPATH=$$OUTPUTPATH/
#exists($$INEXEPATH){
#    OUTLIBPATH=$$replace(OUTLIBPATH,/,$$DIR_SEPARATOR)
#    QMAKE_POST_LINK += $$QMAKE_MKDIR $$OUTLIBPATH $$escape_expand(\n\t)
#    QMAKE_POST_LINK += $$QMAKE_COPY_DIR  $$INEXEPATH  $$OUTEXEPATH $$escape_expand(\n\t)
#}
DESTDIR=$$OUTPUTPATH
LIBS += -L$$OUTPUTPATH/ -lilwiscore



