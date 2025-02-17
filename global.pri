CONFIG(debug, debug|release) {
CONF=debug
}

CONFIG(release, debug|release) {
CONF=release
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
CONFIG += optimize_full
}



win32-msvc* {
    contains(QMAKE_TARGET.arch, x86_64) {
        QMAKE_CXXFLAGS_WARN_ON -= -w34267
        QMAKE_CXXFLAGS += -wd4267
        QMAKE_CXXFLAGS_WARN_ON -= wd4577
        COMPILER = msvc
        BOOST=../external/boost
        PREFIXSHARED=
        PREFIX_COMPILER=msvcx64_
        CORELIBPATH =msvcx64_ilwiscore
        LIB_EXTENSION =$$QMAKE_EXTENSION_SHLIB
        EXTERNAL=../external
    }
}

linux{
    BOOST=/usr/include
    PREFIXSHARED=lib
    LIBS += -L/usr/lib -lgsl \
            -L/usr/lib -lgslcblas
    QMAKE_CFLAGS_ISYSTEM=-I
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-sign-compare
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-deprecated-declarations
    QMAKE_CXXFLAGS += -Wdeprecated-copy
    PREFIX_COMPILER=gccx64_
    CORELIBPATH =gccx64_ilwiscore
    LIB_EXTENSION = so.1.0.0
    DEFINES +=COMPILER_GCC
    EXTERNAL= . #dummy
}

linux-g++{
   !contains(QT_ARCH, x86_64){
       LINUXLIB=/usr/lib/i386-linux-gnu/
    } else {
       LINUXLIB=/usr/lib/x86_64-linux-gnu/
   }
}



QT += sql network concurrent

INCLUDEPATH +=  ./core/ \
                ./core/ilwisobjects \
                ./core/ilwisobjects/geometry \
                ./core/util \
                ./core/ilwisobjects/geometry/geodeticdatum \
                ./core/ilwisobjects/geometry/projection \
                ./core/ilwisobjects/geometry/coordinatesystem \
                ./core/ilwisobjects/geometry/georeference \
                ./core/ilwisobjects/coverage \
                ./core/ilwisobjects/table \
                ./core/ilwisobjects/operation/model \
                ./core/ilwisobjects/operation \
                ./core/ilwisobjects/operation/modeller \
                ./core/ilwisobjects/workflow \
                ./core/ilwisobjects/representation \
                ./core/catalog \
                ./core/ilwisobjects/domain \
                ./core/geos/include \
                $$BOOST \
                $$EXTERNAL

OUTPUTPATH=$$clean_path($$PWD/../output/$$PREFIX_COMPILER/$$CONF/bin)
LIBPATH=$$clean_path($$PWD/../temp/generated/objectfiles/$$PREFIX_COMPILER$$TARGET/$$CONF)
LIBPATH=$$replace(LIBPATH,/,$$DIR_SEPARATOR)
SOURCEDIR=$$clean_path($$PWD/$$TARGET/source)
INLIBPATH=$$LIBPATH/*.$$LIB_EXTENSION
INLIBPATH = $$replace(INLIBPATH,/,$$DIR_SEPARATOR)
OBJECTS_DIR = $$LIBPATH
OUTLIBPATH=dummy $$TARGET
MOC_DIR=$$LIBPATH



equals(TARGET, ilwiscore){
    FOLDER=core
    OUTLIBPATH= $$OUTPUTPATH
}else {
    FOLDER=$$TARGET
    OUTLIBPATH= $$OUTPUTPATH/extensions/$$TARGET
}

exists($$PWD/$$FOLDER/resources){
    SOURCE_RESOURCE = $$clean_path($$PWD/$$FOLDER/resources)
    TARGET_RESOURCE_DIR=$$OUTLIBPATH
    SOURCE_RESOURCE = $$replace(SOURCE_RESOURCE,/,$$DIR_SEPARATOR)
    TARGET_RESOURCE_DIR = $$replace(TARGET_RESOURCE_DIR,/,$$DIR_SEPARATOR)
    !exists($${TARGET_RESOURCE_DIR}) {
        QMAKE_PRE_LINK += $$QMAKE_MKDIR $$TARGET_RESOURCE_DIR $$escape_expand(\n\t)
    }
    QMAKE_PRE_LINK += $$QMAKE_COPY_DIR $$SOURCE_RESOURCE $$TARGET_RESOURCE_DIR $$escape_expand(\n\t)
}
OUTLIBPATH=$$replace(OUTLIBPATH,/,$$DIR_SEPARATOR)
DESTDIR=$$OUTLIBPATH








