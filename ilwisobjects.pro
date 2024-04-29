TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    core.pro \
    baseoperations.pro \
    #commandlineclient.pro \
    featureoperations.pro \
    gdalconnector.pro \
    hydroflow.pro \
    ilwis3connector.pro \
    ilwis4connector.pro \
    ilwisscript.pro \
    internalconnector.pro \
    netcdfconnector.pro \
    projectionimplproj4.pro \
    rasteroperations.pro \
    streamconnector.pro \
    #opencv.pro \
    pythonapi.pro
