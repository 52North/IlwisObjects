#ifndef PYTHONAPI_EXTENSION_H
#define PYTHONAPI_EXTENSION_H

#include "pythonapi_qtGNUTypedefs.h"

#include "../../core/ilwistypes.h"

namespace pythonapi{
    //init modul
    bool _initIlwisObjects(const char * ilwisDir);
    void _exitIlwisObjects();
    void disconnectIssueLogger();
    void connectIssueLogger();
    std::string ilwistype2string(const quint64& type);

    //ilwisTypes
    struct it{
        /*static const quint64 POINT = itPOINT;
        static const quint64 LINE = itLINE;
        static const quint64 POLYGON = itPOLYGON;
        static const quint64 FEATURE = itFEATURE;
        static const quint64 RASTER = itRASTER;
        static const quint64 COVERAGE = itCOVERAGE;
        static const quint64 TABLE = itTABLE;
        static const quint64 GEOREF = itGEOREF;
        static const quint64 COORDSYSTEM = itCOORDSYSTEM;
        static const quint64 ILWDOMAIN = itDOMAIN;
        static const quint64 NUMERICDOMAIN = itNUMERICDOMAIN;
        static const quint64 ITEMDOMAIN = itITEMDOMAIN;
        static const quint64 SINGLEOPERATION = itSINGLEOPERATION;
        static const quint64 WORKFLOW = itWORKFLOW;
        static const quint64 PROJECTION = itPROJECTION;
        static const quint64 ELLIPSOID = itELLIPSOID;
        static const quint64 CATALOG = itCATALOG;
        static const quint64 CONTINUOUSCOLOR = itCONTINUOUSCOLOR;
        static const quint64 PALETTECOLOR = itPALETTECOLOR;
        static const quint64 VALUERANGE = itVALUERANGE;
        static const quint64 COLLECTION = itCOLLECTION;
        static const quint64 ANY = itANY;*/

        static const quint64 UNKNOWN  = itUNKNOWN;
        static const quint64 POINT  = itPOINT;
        static const quint64 LINE  = itLINE;
        static const quint64 POLYGON  = itPOLYGON;
        static const quint64 RASTER  = itRASTER;
        static const quint64 NUMERICDOMAIN  = itNUMERICDOMAIN;
        static const quint64 ITEMDOMAIN  = itITEMDOMAIN;
        static const quint64 COORDDOMAIN  = itCOORDDOMAIN ;
        static const quint64 COLORDOMAIN  = itCOLORDOMAIN ;
        static const quint64 TEXTDOMAIN  = itTEXTDOMAIN ;
        static const quint64 CONVENTIONALCOORDSYSTEM  = itCONVENTIONALCOORDSYSTEM ;
        static const quint64 ORTHOCOORDSYSTEM  = itORTHOCOORDSYSTEM ;
        static const quint64 BOUNDSONLYCSY  = itBOUNDSONLYCSY ;
        static const quint64 ELLIPSOID  = itELLIPSOID ;
        static const quint64 PROJECTION  = itPROJECTION ;
        static const quint64 ATTRIBUTETABLE  = itATTRIBUTETABLE ;
        static const quint64 FLATTABLE  = itFLATTABLE ;
        static const quint64 REPRESENTATION  = itREPRESENTATION ;
        static const quint64 GEOREF  = itGEOREF ;
        static const quint64 SINGLEOPERATION  = itSINGLEOPERATION ;
        static const quint64 CATALOG  = itCATALOG ;
        static const quint64 ENVELOPE  = itENVELOPE ;
        static const quint64 BOUNDINGBOX  = itBOUNDINGBOX ;
        static const quint64 RASTERSIZE  = itRASTERSIZE ;
        static const quint64 GEODETICDATUM  = itGEODETICDATUM ;
        static const quint64 BOOL  = itBOOL ;
        static const quint64 INT8  = itINT8 ;
        static const quint64 UINT8  = itUINT8 ;
        static const quint64 UINT16  = itUINT16 ;
        static const quint64 INT16  = itINT16 ;
        static const quint64 UINT32  = itUINT32 ;
        static const quint64 INT32  = itINT32 ;
        static const quint64 UINT64  = itUINT64 ;
        static const quint64 INT64  = itINT64 ;
        static const quint64 FLOAT  = itFLOAT ;
        static const quint64 DOUBLE  = itDOUBLE ;
        static const quint64 VALUERANGE  = itVALUERANGE ;
        static const quint64 STRING  = itSTRING ;
        static const quint64 DATE  = itDATE ;
        static const quint64 TIME  = itTIME ;
        static const quint64 METRICCOORDINATE  = itMETRICCOORDINATE ;
        static const quint64 PIXEL  = itPIXEL ;
        static const quint64 BINARY  = itBINARY ;
        static const quint64 FILE  = itFILE ;
        static const quint64 URL  = itURL ;
        static const quint64 THEMATICITEM  = itTHEMATICITEM ;
        static const quint64 NAMEDITEM  = itNAMEDITEM ;
        static const quint64 INDEXEDITEM  = itINDEXEDITEM ;
        static const quint64 NUMERICITEM  = itNUMERICITEM ;
        static const quint64 TIMEITEM  = itTIMEITEM ;
        static const quint64 PALETTECOLOR  = itPALETTECOLOR ;
        static const quint64 CONTINUOUSCOLOR  = itCONTINUOUSCOLOR ;
        static const quint64 SAMPLESET  = itSAMPLESET ;
        static const quint64 CATALOGVIEW  = itCATALOGVIEW ;
        static const quint64 COLLECTION  = itCOLLECTION ;
        static const quint64 ARRAY  = itARRAY ;
        static const quint64 WORKFLOW  = itWORKFLOW ;
        static const quint64 WORKSPACE  = itWORKSPACE ;
        static const quint64 LATLON  = itLATLON ;
        static const quint64 COLUMN  = itCOLUMN ;
        static const quint64 SCRIPT  = itSCRIPT ;
        static const quint64 MODEL  = itMODEL ;
        static const quint64 COMBINATIONMATRIX  = itCOMBINATIONMATRIX ;
        static const quint64 LAYER  = itLAYER ;
        static const quint64 CHART  = itCHART ;

        static const quint64 COVERAGE = itPOINT | itRASTER | itPOLYGON | itLINE ; //15
        static const quint64 FEATURE = itPOINT | itPOLYGON | itLINE; //7
        static const quint64 COORDSYSTEM = itCONVENTIONALCOORDSYSTEM | itORTHOCOORDSYSTEM | itBOUNDSONLYCSY; //3584
        static const quint64 TABLE = itFLATTABLE | itATTRIBUTETABLE; //49152
        static const quint64 OPERATIONMETADATA = itSINGLEOPERATION | itWORKFLOW;
        static const quint64 ILWDOMAIN =  itNUMERICDOMAIN | itITEMDOMAIN | itCOORDDOMAIN | itCOLORDOMAIN | itTEXTDOMAIN; //496
        static const quint64 ILWISOBJECT = itCOVERAGE | itDOMAIN | itCOORDSYSTEM | itCATALOG |
                 itELLIPSOID | itPROJECTION | itTABLE | itREPRESENTATION | itGEOREF | itOPERATIONMETADATA | itSCRIPT | itMODEL | itCOMBINATIONMATRIX;
        static const quint64 DATETIME = itDATE | itTIME;
        static const quint64 INTEGER = itINT8 | itUINT8 | itINT16 | itUINT16 | itINT32 | itINT64 | itUINT32 | itUINT64;
        static const quint64 POSITIVEINTEGER = itUINT8 | itUINT16 | itUINT32 | itUINT64;
        static const quint64 NUMBER = itINTEGER | itFLOAT | itDOUBLE | itDATETIME;
        static const quint64 NUMERIC = itNUMBER | itNUMERICITEM | itTIMEITEM;
        static const quint64 DOMAINITEM = itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM | itTIMEITEM | itPALETTECOLOR;
        static const quint64 IDENTIFIERITEM = itNAMEDITEM | itINDEXEDITEM;
        static const quint64 COORDINATE = itMETRICCOORDINATE | itLATLON;

        static const quint64 RESOURCELOCATION = itFILE | itURL;
        static const quint64 LOCATION = itCOORDINATE | itPIXEL;
        static const quint64 COLOR = itCONTINUOUSCOLOR | itPALETTECOLOR;
        static const quint64 ANY = itANY;
    };
}
#endif // PYTHONAPI_EXTENSION_H
