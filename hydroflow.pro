#-------------------------------------------------
#
# Project created by QtCreator 2023-03-07T10:00:24
#
#-------------------------------------------------

TARGET = hydroflow
TEMPLATE = lib

DEFINES += HYDROFLOW_LIBRARY

include(global.pri)
INCLUDEPATH += ./Hydroflow
INCLDUEPATH += /usr/include

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
    Hydroflow/hydroflow_module.cpp \
    Hydroflow/MapFillSinks.cpp \
    Hydroflow/MapFlowDirection.cpp \
    Hydroflow/normalizerelativedem.cpp \
    Hydroflow/relativedem.cpp



HEADERS += \
    Hydroflow/hydroflow_global.h \
    Hydroflow/hydroflow_module.h \
    Hydroflow/MapFillSinks.h \
    Hydroflow/MapFlowDirection.h \
    Hydroflow/normalizerelativedem.h \
    Hydroflow/relativedem.h

DISTFILES += \
    Hydroflow/hydroflow.json

linux{
LIBS += -L$$OUTPUTPATH/ -lilwiscore
}


