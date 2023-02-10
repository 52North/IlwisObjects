#-------------------------------------------------
#
# Project created by QtCreator 2021-03-24T12:24:19
#
#-------------------------------------------------

TARGET = pythonapi
TEMPLATE = lib

DEFINES += PYTHONAPI_LIBRARY

include(global.pri)
INCLUDEPATH += ./pythonapi/
PYMINORVERSION=6
win32{
    PYTHONLOCALDIR=$$clean_path($$PWD/../external/libraries/python/python3$$PYMINORVERSION)
    LIBS += -L$$PYTHONLOCALDIR/libs -lpython3$$PYMINORVERSION
    PYTHON_INCLUDE_FILES=../../external/python3/python3$$PYMINORVERSION/include/
}
linux {
    PYMINORVERSION=8
    PYTHON_INCLUDE_FILES =/usr/include/python3.$$PYMINORVERSION
    LIBS += -L$$/usr/libs -lpython3.$$PYMINORVERSION
    OUTPUTPATHPYTHON=$$clean_path($$PWD/../output/$$PREFIX_COMPILER/$$CONF/ilwispy/ilwis)
}

INCLUDEPATH += $$PYTHON_INCLUDE_FILES
QT += core

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SWIG_PYTHON_INTERPRETER_NO_DEBUG

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    pythonapi/ilwisobjects_wrap.cxx \
    pythonapi/pythonapi_booleanobject.cpp \
    pythonapi/pythonapi_catalog.cpp \
    pythonapi/pythonapi_collection.cpp \
    pythonapi/pythonapi_columndefinition.cpp\
    pythonapi/pythonapi_coordinatesystem.cpp \
    pythonapi/pythonapi_coverage.cpp \
    pythonapi/pythonapi_datadefinition.cpp \
    pythonapi/pythonapi_domain.cpp \
    pythonapi/pythonapi_domainitem.cpp \
    pythonapi/pythonapi_engine.cpp \
    pythonapi/pythonapi_error.cpp \
    pythonapi/pythonapi_extension.cpp \
    pythonapi/pythonapi_feature.cpp \
    pythonapi/pythonapi_featurecoverage.cpp \
    pythonapi/pythonapi_featureiterator.cpp \
    pythonapi/pythonapi_geometry.cpp \
    pythonapi/pythonapi_georeference.cpp \
    pythonapi/pythonapi_ilwisobject.cpp \
    pythonapi/pythonapi_pixeliterator.cpp \
    pythonapi/pythonapi_pyobject.cpp \
    pythonapi/pythonapi_qissuelogger.cpp \
    pythonapi/pythonapi_qvariant.cpp \
    pythonapi/pythonapi_range.cpp \
    pythonapi/pythonapi_rangeiterator.cpp \
    pythonapi/pythonapi_rastercoverage.cpp \
    pythonapi/pythonapi_table.cpp \
    pythonapi/pythonapi_util.cpp \
    pythonapi/pythonapi_vertexiterator.cpp

HEADERS += \
    pythonapi/ilwisobjects.i \
    pythonapi/pythonapi_booleanobject.h \
    pythonapi/pythonapi_catalog.h \
    pythonapi/pythonapi_collection.h \
    pythonapi/pythonapi_columndefinition.h \
    pythonapi/pythonapi_coordinatesystem.h \
    pythonapi/pythonapi_coverage.h \
    pythonapi/pythonapi_datadefinition.h \
    pythonapi/pythonapi_domain.h \
    pythonapi/pythonapi_domainitem.h \
    pythonapi/pythonapi_engine.h \
    pythonapi/pythonapi_error.h \
    pythonapi/pythonapi_extension.h \
    pythonapi/pythonapi_feature.h \
    pythonapi/pythonapi_featurecoverage.h \
    pythonapi/pythonapi_featureiterator.h \
    pythonapi/pythonapi_geometry.h \
    pythonapi/pythonapi_georeference.h \
    pythonapi/pythonapi_global.h \
    pythonapi/pythonapi_ilwisobject.h \
    pythonapi/pythonapi_object.h \
    pythonapi/pythonapi_pixeliterator.h \
    pythonapi/pythonapi_pyobject.h \
    pythonapi/pythonapi_qissuelogger.h \
    pythonapi/pythonapi_qtGNUTypedefs.h \
    pythonapi/pythonapi_qvariant.h \
    pythonapi/pythonapi_range.h \
    pythonapi/pythonapi_rangeiterator.h \
    pythonapi/pythonapi_rastercoverage.h \
    pythonapi/pythonapi_table.h \
    pythonapi/pythonapi_util.h \
    pythonapi/pythonapi_vertexiterator.h \

LIBS += -L$$OUTPUTPATH/ -lilwiscore

COPY_FILES = ilwisobjects.py paths.py

linux {
   swig.target = $$PWD/pythonapi/ilwisobjects.py
    swig.commands = swig3.0 -python -c++ $$PWD/pythonapi/ilwisobjects.i
    swig.depends = $$PWD/pythonapi/ilwisobjects.i $$PWD/pythonapi/*.h $$PWD/pythonapi/*.cpp
    QMAKE_EXTRA_TARGETS += swig
    PRE_TARGETDEPS += $$PWD/pythonapi/ilwisobjects.py
    DEST_DIR = $$clean_path($$DESTDIR)
    #QMAKE_RPATHDIR = $${OUTPUTPATHPYTHON}ilwis
    QMAKE_RPATHDIR += '.'
    QMAKE_POST_LINK += $$quote(test -d $$OUTPUTPATHPYTHON || mkdir -p $$OUTPUTPATHPYTHON$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$quote(cat $$PWD/pythonapi/paths.py $$PWD/pythonapi/ilwisobjects.py > $$DEST_DIR/temp.py$$escape_expand(\n\t))
    QMAKE_POST_LINK += $$QMAKE_MKDIR $${OUTPUTPATHPYTHON} $$escape_expand(\n\t)
    QMAKE_POST_LINK += mv -f $$DEST_DIR/temp.py $${OUTPUTPATHPYTHON}/__init__.py$$escape_expand(\n\t)
    QMAKE_POST_LINK += cp -R -f  $$OUTPUTPATH/extensions/pythonapi/libpythonapi.so.1.0.0 $${OUTPUTPATHPYTHON}/_ilwisobjects.so$$escape_expand(\n\t)
    QMAKE_POST_LINK += cp -R -f  $$OUTPUTPATH/* $${OUTPUTPATHPYTHON}$$escape_expand(\n\t)
    CONFIG(release, debug|release){
        QMAKE_POST_LINK += sh $$PWD/pythonapi/create_whl.sh$$escape_expand(\n\t)
    }

    #CONFIG(release, debug|release){
    #    instfiles.path = $$OUTPUTPATHPYTHON/ilwis
    #    instfiles.files = $$clean_path($$OUTPUTPATH/*)
    #    INSTALLS += instfiles
    #}
}
win32 {
    PYTHONSCRIPT_DIR=$$OUTPUTPATH/extensions/pythonscript/python/Lib/site-packages/ilwis
    PYTHONSCRIPT_DIR=$$replace(PYTHONSCRIPT_DIR,/,$$DIR_SEPARATOR)
    swig.target = $$LIBPATH/ilwisobjects.py
    SWIGPATH = $$clean_path($$PWD/../external/programs/swigwin/swig.exe)
    SWIGPATH =  $$SWIGPATH -python -c++ -outdir $$LIBPATH -o $$clean_path($$PWD/pythonapi/ilwisobjects_wrap.cxx)  $$clean_path($$PWD/pythonapi/ilwisobjects.i)
    swig.commands = $$SWIGPATH
    swig.depends = $$clean_path($$PWD/pythonapi/ilwisobjects.i $$PWD/pythonapi/*.h $$PWD/pythonapi/*.cpp)
    QMAKE_EXTRA_TARGETS += swig
    PRE_TARGETDEPS += $$LIBPATH/ilwisobjects.py
    #next statements are out commented because the windows build will happen ubnder vc, not under qt creator
   # INSTALLS += installer_script installer
   # QMAKE_POST_LINK += $$quote(echo f | xcopy /q /y /i $$LIBPATH\\$${TARGET}.$${QMAKE_EXTENSION_SHLIB} $$PYTHONSCRIPT_DIR\\_ilwisobjects.pyd$$escape_expand(\n\t))
   # QMAKE_POST_LINK += $$quote(copy /y /b $$LIBPATH\\ilwisobjects.py $$PYTHONSCRIPT_DIR\\__init__.py$$escape_expand(\n\t))
}

DISTFILES += \
    pythonapi/create_whl.sh \
    pythonapi/paths.py \
    pythonapi/setup.py


