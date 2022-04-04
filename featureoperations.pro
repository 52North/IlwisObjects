#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = featureoperations
TEMPLATE = lib

DEFINES += FEATUREOPERATIONS_LIBRARY

include(global.pri)
INCLUDEPATH += ./include \
    ./featureoperations/projections

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
    featureoperations/buffer.cpp \
    featureoperations/featureoperationsmodule.cpp \
    featureoperations/gridding.cpp \
    featureoperations/line2polygon.cpp \
    featureoperations/pointrastercrossing.cpp \
    featureoperations/polygon2line.cpp \
    featureoperations/raster2point.cpp \
    featureoperations/raster2polygon.cpp \
    featureoperations/transformcoordinates.cpp


HEADERS += \
 featureoperations/buffer.h \
 featureoperations/featureoperationsmodule.h \
 featureoperations/gridding.h \
 featureoperations/line2polygon.h \
 featureoperations/pointrastercrossing.h \
 featureoperations/polygon2line.h \
 featureoperations/raster2point.h \
 featureoperations/raster2polygon.h \
 featureoperations/transformcoordinates.h



DISTFILES += \
    featureoperations.json
LIBS += -L$$OUTPUTPATH/ -lilwiscore
