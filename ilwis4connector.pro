#-------------------------------------------------
#
# Project created by QtCreator 2021-03-23T10:50:24
#
#-------------------------------------------------

TARGET = ilwis4connector
TEMPLATE = lib

DEFINES += ILWIS4CONNECTOR_LIBRARY

include(global.pri)
INCLUDEPATH += ./include \
    ./ilwis4connector/qtcsv \
    ../external \
    $$EXTERNAL/zlib-1.2.11

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
   ilwis4connector/ilwis4catalogexplorer.cpp \
   ilwis4connector/ilwis4connector.cpp \
   ilwis4connector/ilwis4coordinatesystemconnector.cpp \
   ilwis4connector/ilwis4domainconnector.cpp \
   ilwis4connector/ilwis4featureconnector.cpp \
   ilwis4connector/ilwis4georefconnector.cpp \
   ilwis4connector/ilwis4objectfactory.cpp \
   ilwis4connector/ilwis4rasterconnector.cpp \
   ilwis4connector/ilwis4representationconnector.cpp \
   ilwis4connector/ilwis4scriptconnector.cpp \
   ilwis4connector/ilwis4tableconnector.cpp \
   ilwis4connector/ilwis4workflowconnector.cpp \
   ilwis4connector/iwis4connectormodule.cpp \
   ilwis4connector/qtcsv/sources/contentiterator.cpp \
   ilwis4connector/qtcsv/sources/reader.cpp \
   ilwis4connector/qtcsv/sources/stringdata.cpp \
   ilwis4connector/qtcsv/sources/variantdata.cpp \
   ilwis4connector/qtcsv/sources/writer.cpp

HEADERS += \
   \
  ilwis4connector/ilwis4catalogexplorer.h \
  ilwis4connector/ilwis4connector.h \
  ilwis4connector/ilwis4coordinatesystemconnector.h \
  ilwis4connector/ilwis4domainconnector.h \
  ilwis4connector/ilwis4featureconnector.h \
  ilwis4connector/ilwis4georefconnector.h \
  ilwis4connector/ilwis4objectfactory.h \
  ilwis4connector/ilwis4rasterconnector.h \
  ilwis4connector/ilwis4representationconnector.h \
  ilwis4connector/ilwis4scriptconnector.h \
  ilwis4connector/ilwis4tableconnector.h \
  ilwis4connector/ilwis4workflowconnector.h \
  ilwis4connector/iwis4connectormodule.h \
  ilwis4connector/qtcsv/include/abstractdata.h \
  ilwis4connector/qtcsv/include/qtcsv_global.h \
  ilwis4connector/qtcsv/include/reader.h \
  ilwis4connector/qtcsv/include/stringdata.h \
  ilwis4connector/qtcsv/include/variantdata.h \
  ilwis4connector/qtcsv/include/writer.h \
  ilwis4connector/qtcsv/include/contentiterator.h \
  ilwis4connector/qtcsv/include/filechecker.h \
  ilwis4connector/qtcsv/include/symbols.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
win32{
LIBS += -L../../external/libraries/Debug/ -lquazip
message($$LIBS)
}

DISTFILES += \
    ilwis4connector.json \
    resources/ilwis4formats.config

LIBS += -L$$OUTPUTPATH/ -lilwiscore
