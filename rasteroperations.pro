#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = rasteroperations
TEMPLATE = lib

DEFINES += RASTEROPERATIONS_LIBRARY

include(global.pri)
INCLUDEPATH += ./rasteroperations


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
 rasteroperations/aggregateraster.cpp \
 rasteroperations/aggregaterasterstatistics.cpp \
 rasteroperations/classification/areanumbering.cpp \
 rasteroperations/classification/classifier.cpp \
 rasteroperations/classification/clusterraster.cpp \
 rasteroperations/classification/comboclassification.cpp \
 rasteroperations/crossrasters.cpp \
 rasteroperations/densifyraster.cpp \
 rasteroperations/distanceraster.cpp \
 rasteroperations/line2raster.cpp \
 rasteroperations/filter/linearrasterfilter.cpp \
 rasteroperations/lookupindex.cpp \
 rasteroperations/mannkendallsignificancetest.cpp \
 rasteroperations/mirrorrotateraster.cpp \
 rasteroperations/movingaverage.cpp \
 rasteroperations/percentilegroups.cpp \
 rasteroperations/percentilegroupscolumn.cpp \
 rasteroperations/pointtoraster.cpp \
 rasteroperations/polygontoraster.cpp \
 rasteroperations/filter/rankorderrasterfilter.cpp \
 rasteroperations/classification/rasterclassification.cpp \
 rasteroperations/rasteroperationsmodule.cpp \
 rasteroperations/classification/rasterslicing.cpp \
 rasteroperations/rasterstackfromattributes.cpp \
 rasteroperations/rasterstretchoperation.cpp \
 rasteroperations/rastertopoint.cpp \
 rasteroperations/setrepresentation.cpp \
 rasteroperations/filter/stackminmaxfilter.cpp \
 rasteroperations/filter/timesat_nrs.cpp \
 rasteroperations/zonalstatistics.cpp

HEADERS += \ \
    rasteroperations/aggregateraster.h \
    rasteroperations/aggregaterasterstatistics.h \
    rasteroperations/classification/areanumbering.h \
    rasteroperations/classification/classifier.h \
    rasteroperations/classification/clusterraster.h \
    rasteroperations/classification/comboclassification.h \
    rasteroperations/crossrasters.h \
    rasteroperations/densifyraster.h \
    rasteroperations/distanceraster.h \
    rasteroperations/line2raster.h \
    rasteroperations/filter/linearrasterfilter.h \
    rasteroperations/lookupindex.h \
    rasteroperations/mannkendallsignificancetest.h \
    rasteroperations/mirrorrotateraster.h \
    rasteroperations/movingaverage.h \
    rasteroperations/percentilegroups.h \
    rasteroperations/percentilegroupscolumn.h \
    rasteroperations/pointtoraster.h \
    rasteroperations/polygontoraster.h \
    rasteroperations/filter/rankorderrasterfilter.h \
    rasteroperations/classification/rasterclassification.h \
    rasteroperations/rasteroperationsmodule.h \
    rasteroperations/classification/rasterslicing.h \
    rasteroperations/rasterstackfromattributes.h \
    rasteroperations/rasterstrechoperation.h \
    rasteroperations/rastertopoint.h \
    rasteroperations/setrepresentation.h \
    rasteroperations/filter/stackminmaxfilter.h \
    rasteroperations/filter/timesat_nrs.h \
    rasteroperations/zonalstatistics.h


DISTFILES += \
    rasteroperations.json

LIBS += -L$$OUTPUTPATH/ -lilwiscore
