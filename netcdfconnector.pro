#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = netcdfconnector
TEMPLATE = lib

DEFINES += NETCDF_LIBRARY

include(global.pri)
INCLUDEPATH += ./netcdfconnector
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
    netcdfconnector/netcdfcatalogconnector.cpp \
    netcdfconnector/netcdfmodule.cpp \
    netcdfconnector/netcdfobjectfactory.cpp \
    netcdfconnector/netcdfrasterconnector.cpp



HEADERS += \
    netcdfconnector/netcdfcatalogconnector.h \
    netcdfconnector/netcdfmodule.h \
    netcdfconnector/netcdfobjectfactory.h \
    netcdfconnector/netcdfrasterconnector.h


DISTFILES += \
    netcdfconnector/netcdfconnector.json \
    resources/netcdfformats.config

linux{
LIBS += -L$$OUTPUTPATH/ -lilwiscore
LIBS += -L/lib/x86_64-linux-gnu/ -lnetcdf
LIBS += -L/lib/x86_64-linux-gnu/ -lnetcdf_c++4
}


