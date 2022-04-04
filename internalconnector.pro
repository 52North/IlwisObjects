#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = internalconnector
TEMPLATE = lib

DEFINES += INTERNALCONNECTOR_LIBRARY

include(global.pri)
INCLUDEPATH += ./internalconnector \
    ./internalconnector/projections

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
    internalconnector/internalcatalogexplorer.cpp \
    internalconnector/internalcombinationmatrixconnector.cpp \
    internalconnector/internalcoordinatesystemconnector.cpp \
    internalconnector/internaldomain.cpp \
    internalconnector/internalellipsoidconnector.cpp \
    internalconnector/internalfeaturecoverageconnector.cpp \
    internalconnector/internalgeoreferenceconnector.cpp \
    internalconnector/internalilwisobjectfactory.cpp \
    internalconnector/internalmodelconnector.cpp \
    internalconnector/internalmodule.cpp \
    internalconnector/internalprjimplfactory.cpp \
    internalconnector/internalprojectionconnector.cpp \
    internalconnector/internalrastercoverageconnector.cpp \
    internalconnector/internalrepresentation.cpp \
    internalconnector/internalscriptconnector.cpp \
    internalconnector/internaltableconnector.cpp \
    internalconnector/operationmetadataconnector.cpp \
    internalconnector/projections/cylindsinusinterrupt2.cpp \
    internalconnector/projections/dutchrd.cpp \
    internalconnector/projections/platecaree.cpp \
    internalconnector/projections/projectionimplementationinternal.cpp \
    internalconnector/workflowconnector.cpp

HEADERS += \
    internalconnector/epsg.h \
    internalconnector/internalcatalogexplorer.h \
    internalconnector/internalcombinationmatrixconnector.h \
    internalconnector/internalcoordinatesystemconnector.h \
    internalconnector/internaldomain.h \
    internalconnector/internalellipsoidconnector.h \
    internalconnector/internalfeaturecoverageconnector.h \
    internalconnector/internalgeoreferenceconnector.h \
    internalconnector/internalilwisobjectfactory.h \
    internalconnector/internalmodelconnector.h \
    internalconnector/internalmodule.h \
    internalconnector/internalprjmplfactory.h \
    internalconnector/internalprojectionconnector.h \
    internalconnector/internalrastercoverageconnector.h \
    internalconnector/internalrepresentation.h \
    internalconnector/internalscriptconnector.h \
    internalconnector/internaltableconnector.h \
    internalconnector/operationmetadataconnector.h \
    internalconnector/projections/cylindsinusinterrupt2.h \
    internalconnector/projections/dutchrd.h \
    internalconnector/projections/platecaree.h \
    internalconnector/projections/projectionimplementationinternal.h \
    internalconnector/workflowconnector.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    internalconnector.json

LIBS += -L$$OUTPUTPATH/ -lilwiscore
