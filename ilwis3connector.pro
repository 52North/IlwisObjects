#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = ilwis3connector
TEMPLATE = lib

DEFINES += ILWIS3CONNECTOR_LIBRARY

include(global.pri)
INCLUDEPATH += ./ilwis3connector


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
    \
    ilwis3connector/RawConverter.cpp \
    ilwis3connector/binaryilwis3table.cpp \
    ilwis3connector/coordinatesystemconnector.cpp \
    ilwis3connector/coverageconnector.cpp \
    ilwis3connector/domainconnector.cpp \
    ilwis3connector/ellipsoidconnector.cpp \
    ilwis3connector/georefconnector.cpp \
    ilwis3connector/ilwis3catalogexplorer.cpp \
    ilwis3connector/ilwis3catalogfileexplorer.cpp \
    ilwis3connector/ilwis3connector.cpp \
    ilwis3connector/ilwis3featureconnector.cpp \
    ilwis3connector/ilwis3module.cpp \
    ilwis3connector/ilwis3objectfactory.cpp \
    ilwis3connector/ilwis3projectionconnector.cpp \
    ilwis3connector/ilwis3range.cpp \
    ilwis3connector/ilwis3representationconnector.cpp \
    ilwis3connector/ilwisrastercoverageconnector.cpp \
    ilwis3connector/inifile.cpp \
    ilwis3connector/odfitem.cpp \
    ilwis3connector/tableconnector.cpp
  

HEADERS += \
   \
    ilwis3connector/Ilwis3Connector_global.h \
    ilwis3connector/binaryilwis3table.h \
    ilwis3connector/coordinatesystemconnector.h \
    ilwis3connector/coverageconnector.h \
    ilwis3connector/domainconnector.h \
    ilwis3connector/ellipsoidconnector.h \
    ilwis3connector/georefconnector.h \
    ilwis3connector/ilwis3catalogconnector.h \
    ilwis3connector/ilwis3catalogexplorer.h \
    ilwis3connector/ilwis3catalogfileexplorer.h \
    ilwis3connector/ilwis3connector.h \
    ilwis3connector/ilwis3featureconnector.h \
    ilwis3connector/ilwis3module.h \
    ilwis3connector/ilwis3objectfactory.h \
    ilwis3connector/ilwis3projectionconnector.h \
    ilwis3connector/ilwis3range.h \
    ilwis3connector/ilwis3representationconnector.h \
    ilwis3connector/ilwisrastercoverageconnector.h \
    ilwis3connector/inifile.h \
    ilwis3connector/odfitem.h \
    ilwis3connector/rawconverter.h \
    ilwis3connector/tableconnector.h
 

unix {
    target.path = /usr/lib
    INSTALLS += target
}
win32{
LIBS += -L../../external/libraries/Debug/ -lquazip
message($$LIBS)
}

DISTFILES += \
    ilwis3connector/ilwis3connector.json \

LIBS += -L$$OUTPUTPATH/ -lilwiscore
