#-------------------------------------------------
#
# Project created by QtCreator 2021-03-19T08:50:35
#
#-------------------------------------------------

TARGET = baseoperations
TEMPLATE = lib

include(global.pri)
INCLUDEPATH += ./baseoperations \
    ./baseoperations/data \
    ./baseoperations/conditions \
    ./baseoperations/creators \
    ./includes/features \
    ./baseoperations/geometry \
    ./baseoperations/math \
    ./baseoperations/util


DEFINES += BASEOPERATIONS_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    baseoperations.json

HEADERS += \
    baseoperations/conditions/compare.h \
    baseoperations/conditions/iscompatible.h \
    baseoperations/conditions/junction.h \
    baseoperations/conditions/numbercondition.h \
    baseoperations/conditions/rangeoperation.h \
    baseoperations/creators/createboundsonlycsy.h \
    baseoperations/creators/createcombinationmatrix.h \
    baseoperations/creators/createcornersgeoreference.h \
    baseoperations/creators/createfeaturecoverage.h \
    baseoperations/creators/createidentifierdomain.h \
    baseoperations/creators/createintervaldomain.h \
    baseoperations/creators/createitemrepresentation.h \
    baseoperations/creators/createnumericdomain.h \
    baseoperations/creators/createpalettedomain.h \
    baseoperations/creators/createprojectedcoordinatesystem.h \
    baseoperations/creators/createrastercoverage.h \
    baseoperations/creators/createtable.h \
    baseoperations/creators/createthematicdomain.h \
    baseoperations/creators/createtimedomain.h \
    baseoperations/creators/createtimeintervaldomain.h \
    baseoperations/creators/createvaluerepresentation.h \
    baseoperations/creators/parsestackdefinition.h \
    baseoperations/data/addcolumn.h \
    baseoperations/data/addrasterband.h \
    baseoperations/data/aggregatetable.h \
    baseoperations/data/assignment.h \
    baseoperations/data/connect.h \
    baseoperations/data/convertcolumndomain.h \
    baseoperations/data/copycolumn.h \
    baseoperations/data/iffeature.h \
    baseoperations/data/iffraster.h \
    baseoperations/data/ifoperation.h \
    baseoperations/data/number2string.h \
    baseoperations/data/property.h \
    baseoperations/data/rasvalue.h \
    baseoperations/data/saveas.h \
    baseoperations/data/selection.h \
    baseoperations/data/selectionbase.h \
    baseoperations/data/selectionfeatures.h \
    baseoperations/data/selectiontable.h \
    baseoperations/data/serviceobject.h \
    baseoperations/data/setattributetabel.h \
    baseoperations/data/setvaluerange.h \
    baseoperations/data/setvariable.h \
    baseoperations/data/systemproperty.h \
    baseoperations/data/tablevalue.h \
    baseoperations/data/tablevaluebyprimarykey.h \
    baseoperations/feature/geometryoperations.h \
    baseoperations/geometry/coord2pixel.h \
    baseoperations/geometry/fcoordinate.h \
    baseoperations/geometry/fpixel.h \
    baseoperations/geometry/gridsize.h \
    baseoperations/geometry/mastergeoreference.h \
    baseoperations/geometry/pixel2coord.h \
    baseoperations/geometry/resampleraster.h \
    baseoperations/geometry/setgeoreference.h \
    baseoperations/geometry/spatialrelation.h \
    baseoperations/math/aggregation.h \
    baseoperations/math/binarylogical.h \
    baseoperations/math/binarymathfeature.h \
    baseoperations/math/binarymathraster.h \
    baseoperations/math/binarymathtable.h \
    baseoperations/math/calculatoroperation.h \
    baseoperations/math/columnjoin.h \
    baseoperations/math/columnunaryoperation.h \
    baseoperations/math/mapcalc.h \
    baseoperations/math/tabcalc.h \
    baseoperations/math/unarymath.h \
    baseoperations/math/unarymathoperations.h \
    baseoperations/math/unarymathrasterandnumber.h \
    baseoperations/math/unarymathtable.h \
    baseoperations/util/stringoperations.h \
    baseoperations/util/testoperation.h \
    baseoperations/util/text2output.h \
    baseoperations/util/workingcatalog.h \
    baseoperations/baseoperations_global.h \
    baseoperations/baseoperationsmodule.h

SOURCES += \
    baseoperations/conditions/compare.cpp \
    baseoperations/conditions/iscompatible.cpp \
    baseoperations/conditions/junction.cpp \
    baseoperations/conditions/numbercondition.cpp \
    baseoperations/conditions/rangeoperation.cpp \
    baseoperations/creators/createboundsonlycsy.cpp \
    baseoperations/creators/createcombinationmatrix.cpp \
    baseoperations/creators/createcornersgeoreference.cpp \
    baseoperations/creators/createfeaturecoverage.cpp \
    baseoperations/creators/createidentifierdomain.cpp \
    baseoperations/creators/createintervaldomain.cpp \
    baseoperations/creators/createitemrepresentation.cpp \
    baseoperations/creators/createnumericdomain.cpp \
    baseoperations/creators/createpalettedomain.cpp \
    baseoperations/creators/createprojectedcoordinatesystem.cpp \
    baseoperations/creators/createrastercoverage.cpp \
    baseoperations/creators/createtable.cpp \
    baseoperations/creators/createthematicdomain.cpp \
    baseoperations/creators/createtimedomain.cpp \
    baseoperations/creators/createtimeintervaldomain.cpp \
    baseoperations/creators/createvaluerepresentation.cpp \
    baseoperations/creators/parsestackdefinition.cpp \
    baseoperations/data/addcolumn.cpp \
    baseoperations/data/addrasterband.cpp \
    baseoperations/data/aggregatetable.cpp \
    baseoperations/data/assignment.cpp \
    baseoperations/data/connect.cpp \
    baseoperations/data/convertcolumndomain.cpp \
    baseoperations/data/copycolumn.cpp \
    baseoperations/data/iffeature.cpp \
    baseoperations/data/iffraster.cpp \
    baseoperations/data/ifoperation.cpp \
    baseoperations/data/number2string.cpp \
    baseoperations/data/property.cpp \
    baseoperations/data/rasvalue.cpp \
    baseoperations/data/saveas.cpp \
    baseoperations/data/selection.cpp \
    baseoperations/data/selectionbase.cpp \
    baseoperations/data/selectionfeatures.cpp \
    baseoperations/data/selectiontable.cpp \
    baseoperations/data/serviceobject.cpp \
    baseoperations/data/setattributetabel.cpp \
    baseoperations/data/setvaluerange.cpp \
    baseoperations/data/setvariable.cpp \
    baseoperations/data/systemproperty.cpp \
    baseoperations/data/tablevalue.cpp \
    baseoperations/data/tablevaluebyprimarykey.cpp \
    baseoperations/geometry/coord2pixel.cpp \
    baseoperations/geometry/fcoordinate.cpp \
    baseoperations/geometry/fpixel.cpp \
    baseoperations/geometry/gridsize.cpp \
    baseoperations/geometry/mastergeoreference.cpp \
    baseoperations/geometry/pixel2coord.cpp \
    baseoperations/geometry/resampleraster.cpp \
    baseoperations/geometry/setgeoreference.cpp \
    baseoperations/geometry/spatialrelation.cpp \
    baseoperations/math/aggregation.cpp \
    baseoperations/math/binarylogical.cpp \
    baseoperations/math/binarymathfeature.cpp \
    baseoperations/math/binarymathraster.cpp \
    baseoperations/math/binarymathtable.cpp \
    baseoperations/math/calculatoroperation.cpp \
    baseoperations/math/columnjoin.cpp \
    baseoperations/math/columnunaryoperation.cpp \
    baseoperations/math/mapcalc.cpp \
    baseoperations/math/tabcalc.cpp \
    baseoperations/math/unarymath.cpp \
    baseoperations/math/unarymathoperations.cpp \
    baseoperations/math/unarymathrasterandnumber.cpp \
    baseoperations/math/unarymathtable.cpp \
    baseoperations/util/stringoperations.cpp \
    baseoperations/util/testoperation.cpp \
    baseoperations/util/text2output.cpp \
    baseoperations/util/workingcatalog.cpp \
    baseoperations/baseoperationsmodule.cpp

LIBS += -L$$OUTPUTPATH/ -lilwiscore
#LIBS += -Lc:/ilwisobjects/proversion/generated/msvc64bit_debug_core/debug/ -lilwiscore

