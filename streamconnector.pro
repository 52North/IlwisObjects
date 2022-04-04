#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = streamconnector
TEMPLATE = lib

DEFINES += STREAMCONNECTOR_LIBRARY

include(global.pri)
INCLUDEPATH += ./streamconnector

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    streamconnector/catalogconnection.cpp \
    streamconnector/catalogserializerv1.cpp \
    streamconnector/combinationmatrixserializerv1.cpp \
    streamconnector/coordinatesystemserializerv1.cpp \
    streamconnector/coverageserializerv1.cpp \
    streamconnector/domainserializerv1.cpp \
    streamconnector/downloadmanager.cpp \
    streamconnector/ellipsoidserializerv1.cpp \
    streamconnector/featureserializerv1.cpp \
    streamconnector/georefserializerv1.cpp \
    streamconnector/modelserializerv1.cpp \
    streamconnector/operationmetadataserializerv1.cpp \
    streamconnector/projectionserializerv1.cpp \
    streamconnector/rasterserializerv1.cpp \
    streamconnector/rawconverter.cpp \
    streamconnector/representationserializer.cpp \
    streamconnector/scriptserializerv1.cpp \
    streamconnector/streamcatalogexplorer.cpp \
    streamconnector/streamconnector.cpp \
    streamconnector/streammodule.cpp \
    streamconnector/streamobjectfactory.cpp \
    streamconnector/tableserializerv1.cpp \
    streamconnector/versioneddatastreamfactory.cpp \
    streamconnector/versionedserializer.cpp \
    streamconnector/workflowserializerv1.cpp


HEADERS += \
    streamconnector/catalogconnection.h \
    streamconnector/catalogserializerv1.h \
    streamconnector/combinationmatrixserializerv1.h \
    streamconnector/coordinatesystemserializerv1.h \
    streamconnector/coverageserializerv1.h \
    streamconnector/domainserializerv1.h \
    streamconnector/downloadmanager.h \
    streamconnector/ellipsoidserializerv1.h \
    streamconnector/featureserializerv1.h \
    streamconnector/georefserializerv1.h \
    streamconnector/modelserializerv1.h \
    streamconnector/operationmetadataserializerv1.h \
    streamconnector/projectionserializerv1.h \
    streamconnector/rasterserializerv1.h \
    streamconnector/rawconverter.h \
    streamconnector/representationserializer.h \
    streamconnector/scriptserializerv1.h \
    streamconnector/streamcatalogexplorer.h \
    streamconnector/streamconnector.h \
    streamconnector/streammodule.h \
    streamconnector/streamobjectfactory.h \
    streamconnector/tableserializerv1.h \
    streamconnector/versioneddatastreamfactory.h \
    streamconnector/versionedserializer.h \
    streamconnector/workflowserializerv1.h


DISTFILES += \
    streamconnector.json \
    resources/streamformats.config

LIBS += -L$$OUTPUTPATH/ -lilwiscore
