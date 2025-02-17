/* The ILWIS SWIG interface file*/

%module(docstring="The Python API for ILWIS Objects") ilwisobjects

%feature("autodoc","1");

%include "exception.i"
%include "std_string.i"

%begin %{
   #include <cmath>
%}

%{
#include "kernel.h"
#include "ilwisdata.h"
#include "util/range.h"
#include "itemrange.h"
#include "catalog.h"
#include "coverage.h"
#include "feature.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_error.h"
#include "pythonapi_extension.h"
#include "pythonapi_object.h"
#include "pythonapi_engine.h"
#include "pythonapi_collection.h"
#include "pythonapi_ilwisobject.h"
#include "pythonapi_coordinatesystem.h"
#include "pythonapi_table.h"
#include "pythonapi_coverage.h"
#include "pythonapi_object.h"
#include "pythonapi_util.h"
#include "pythonapi_geometry.h"
#include "pythonapi_feature.h"
#include "pythonapi_featureiterator.h"
#include "pythonapi_featurecoverage.h"
#include "pythonapi_pixeliterator.h"
#include "pythonapi_georeference.h"
#include "pythonapi_rastercoverage.h"
#include "pythonapi_range.h"
#include "pythonapi_catalog.h"
#include "pythonapi_domain.h"
#include "pythonapi_datadefinition.h"
#include "pythonapi_columndefinition.h"
#include "pythonapi_domainitem.h"
#include "pythonapi_rangeiterator.h"
#include "pythonapi_vertexiterator.h"
%}

%include "pythonapi_qtGNUTypedefs.h"

%newobject pythonapi::Engine::_do; // hint to swig that ilwis.do() returns objects that should be garbage-collected
%newobject pythonapi::Collection::_getitem;

%init %{
    //init FeatureCreationError for Python
    pythonapi::featureCreationError = PyErr_NewException("_ilwisobjects.FeatureCreationError",NULL,NULL);
    Py_INCREF(pythonapi::featureCreationError);
    PyModule_AddObject(m, "FeatureCreationError", pythonapi::featureCreationError);//m is SWIG declaration for Python C API modul creation
    //init IlwisException for Python
    pythonapi::ilwisException = PyErr_NewException("_ilwisobjects.IlwisException",NULL,NULL);
    Py_INCREF(pythonapi::ilwisException);
    PyModule_AddObject(m, "IlwisException", pythonapi::ilwisException);//m is SWIG declaration for Python C API modul creation
    //init InvalidObjectException for Python
    pythonapi::invalidObjectException = PyErr_NewException("_ilwisobjects.InvalidObjectException",NULL,NULL);
    Py_INCREF(pythonapi::invalidObjectException);
    PyModule_AddObject(m, "InvalidObjectException", pythonapi::invalidObjectException);//m is SWIG declaration for Python C API modul creation
    atexit(exitPython);
%}

%{
void exitPython()
{
    pythonapi::_exitIlwisObjects();
}
%}

%{
std::string ilwistype2string(quint64 stype)
{
    return pythonapi::_ilwistype2string(stype);
}
%}

%pythoncode %{
   def type2string(stype):
        return _ilwistype2string(stype);
%}

//adds the export flag to pyd library for the IlwisException
%pythoncode %{
    IlwisException = _ilwisobjects.IlwisException
    InvalidObjectException = _ilwisobjects.InvalidObjectException
    FeatureCreationError = _ilwisobjects.FeatureCreationError
    try:
        if not path is None:
            err = _ilwisobjects._initIlwisObjects(path)
            if len(err) > 0:
                raise ImportError("ILWIS couldn't be initialized!\n" + err)
    except NameError:
        err = _ilwisobjects._initIlwisObjects("")
        if len(err) > 0:
            raise ImportError("ILWIS couldn't be initialized!\n" + err)
%}
//catch std::exception's on all C API function calls
%exception{
    try {
        $action
    }catch (std::exception& e) {
        PyErr_SetString(pythonapi::translate_Exception_type(e),pythonapi::get_err_message(e));
        SWIG_fail;
    }
}


%include "pythonapi_extension.h"

%pythoncode %{
def object_cast(obj):
    type = obj.ilwisType()
    prevThisown = obj.thisown
    obj.thisown = False # temporarily disable garbage collection on this object; the "cast" functions below return the same C++ pointer, but swig seems to garbage-collect the original if we don't do so
    if it.RASTER & type != 0:
        obj = RasterCoverage.toRasterCoverage(obj)
    elif it.FEATURE & type != 0:
        obj = FeatureCoverage.toFeatureCoverage(obj)
    elif it.GEOREF & type != 0:
        obj = GeoReference.toGeoReference(obj)
    elif it.TABLE & type != 0:
        obj = Table.toTable(obj)
    elif it.NUMERICDOMAIN & type != 0:
      obj = NumericDomain.toNumericDomain(obj)
    elif it.ILWDOMAIN & type != 0:
      obj = Domain.toDomain(obj)
    elif it.COORDSYSTEM & type != 0:
        obj = CoordinateSystem.toCoordinateSystem(obj)
#    elif it.OPERATIONMETADATA & type != 0:
#        obj = OperationMetaData.toOperationMetaData(obj)
#    elif it.PROJECTION & type != 0:
#        obj = Projection.toProjection(obj)
#    elif it.ELLIPSOID & type != 0:
#        obj = Ellipsoid.toEllipsoid(obj)
    elif it.CATALOG & type != 0:
        obj = Catalog.toCatalog(obj)
    elif it.COLLECTION & type != 0:
        obj = Collection.toCollection(obj)
        obj._fetchItems()
    elif type == 0:
        raise TypeError("unknown IlwisType")
    obj.thisown = prevThisown # re-enable garbage collection if it was available (needs investigation if it wasn't)
    return obj
%}

%include "pythonapi_object.h"

%include "pythonapi_engine.h"

%extend pythonapi::Engine {
%insert("python") %{
    @staticmethod
    def do(operation,arg1="",arg2="",arg3="",arg4="",arg5="",arg6="",arg7="",arg8="",arg9="",out=""):
        if str(operation) != "":
            if (type(arg1)==str and len(arg1) > 0):
                arg1 = "'" + arg1 + "'"
            if (type(arg2)==str and len(arg2) > 0):
                arg2 = "'" + arg2 + "'"
            if (type(arg3)==str and len(arg3) > 0):
                arg3 = "'" + arg3 + "'"
            if (type(arg4)==str and len(arg4) > 0):
                arg4 = "'" + arg4 + "'"
            if (type(arg5)==str and len(arg5) > 0):
                arg5 = "'" + arg5 + "'"
            if (type(arg6)==str and len(arg6) > 0):
                arg6 = "'" + arg6 + "'"
            if (type(arg7)==str and len(arg7) > 0):
                arg7 = "'" + arg7 + "'"
            if (type(arg8)==str and len(arg8) > 0):
                arg8 = "'" + arg8 + "'"
            if (type(arg9)==str and len(arg9) > 0):
                arg9 = "'" + arg9 + "'"
            obj = Engine__do(str(out),str(operation),str(arg1),str(arg2),str(arg3),str(arg4),str(arg5),str(arg6),str(arg7),str(arg8),str(arg9))
        else:
            raise IlwisException("no operation given!")
        return object_cast(obj)

    @staticmethod
    def do2(operation,arg1="",arg2="",arg3="",arg4="",arg5="",arg6="",arg7="",arg8="",arg9="",out=""):
        if str(operation) != "":
            if (type(arg1)==str and len(arg1) > 0):
                arg1 = "'" + arg1 + "'"
            if (type(arg2)==str and len(arg2) > 0):
                arg2 = "'" + arg2 + "'"
            if (type(arg3)==str and len(arg3) > 0):
                arg3 = "'" + arg3 + "'"
            if (type(arg4)==str and len(arg4) > 0):
                arg4 = "'" + arg4 + "'"
            if (type(arg5)==str and len(arg5) > 0):
                arg5 = "'" + arg5 + "'"
            if (type(arg6)==str and len(arg6) > 0):
                arg6 = "'" + arg6 + "'"
            if (type(arg7)==str and len(arg7) > 0):
                arg7 = "'" + arg7 + "'"
            if (type(arg8)==str and len(arg8) > 0):
                arg8 = "'" + arg8 + "'"
            if (type(arg9)==str and len(arg9) > 0):
                arg9 = "'" + arg9 + "'"
            obj = Engine__do2(str(out),str(operation),str(arg1),str(arg2),str(arg3),str(arg4),str(arg5),str(arg6),str(arg7),str(arg8),str(arg9))
        else:
            raise IlwisException("no operation given!")
        return obj

    @staticmethod
    def catalogItems(filter):
        return sorted(Engine__catalogItems(filter), key = str.lower)
%}
}

%include "pythonapi_collection.h"
%extend pythonapi::Collection {
%insert("python") %{
def _fetchItems(self):
    self._it = []
    for i in range(0, self.__len__()):
        self._it.append(object_cast(self._getitem(i)))
def __getitem__(self, name):
    return self._it[name]
%}
}

%include "pythonapi_ilwisobject.h"

%include "pythonapi_coordinatesystem.h"

%include "pythonapi_util.h"

%template(Pixel) pythonapi::PixelTemplate<qint32>;
%template(PixelD) pythonapi::PixelTemplate<double>;
%template(Size) pythonapi::SizeTemplate<quint32>;
%template(SizeD) pythonapi::SizeTemplate<double>;
%template(Box) pythonapi::BoxTemplate<Ilwis::Location<qint32, false>, pythonapi::PixelTemplate<qint32>, quint32>;
%template(Envelope) pythonapi::BoxTemplate<Ilwis::Coordinate, pythonapi::Coordinate, double>;
%template(NumericStatistics) pythonapi::ContainerStatistics<double>;

%include "pythonapi_table.h"

%include "pythonapi_coverage.h"

%include "pythonapi_object.h"


%extend pythonapi::SizeTemplate<quint32> {
%insert("python") %{
    __swig_getmethods__["xsize"] = xsize
    __swig_getmethods__["ysize"] = ysize
    __swig_getmethods__["zsize"] = zsize
    __swig_setmethods__["xsize"] = setXsize
    __swig_setmethods__["ysize"] = setYsize
    __swig_setmethods__["zsize"] = setZsize
    if _newclass:
        xsize = property(xsize,setXsize)
        ysize = property(ysize,setYsize)
        zsize = property(zsize,setZsize)
%}
}
%extend pythonapi::SizeTemplate<double> {
%insert("python") %{
    __swig_getmethods__["xsize"] = xsize
    __swig_getmethods__["ysize"] = ysize
    __swig_getmethods__["zsize"] = zsize
    __swig_setmethods__["xsize"] = setXsize
    __swig_setmethods__["ysize"] = setYsize
    __swig_setmethods__["zsize"] = setZsize
    if _newclass:
        xsize = property(xsize,setXsize)
        ysize = property(ysize,setYsize)
        zsize = property(zsize,setZsize)
%}
}
%extend pythonapi::PixelTemplate<qint32> {//Pixel
%insert("python") %{
    __swig_getmethods__["x"] = x
    __swig_getmethods__["y"] = y
    __swig_getmethods__["z"] = z
    __swig_setmethods__["x"] = setX
    __swig_setmethods__["y"] = setY
    __swig_setmethods__["z"] = setZ
    if _newclass:
        x = property(x,setX)
        y = property(y,setY)
        z = property(z,setZ)
%}
}
%extend pythonapi::PixelTemplate<double> {//PixelD
%insert("python") %{
    __swig_getmethods__["x"] = x
    __swig_getmethods__["y"] = y
    __swig_getmethods__["z"] = z
    __swig_setmethods__["x"] = setX
    __swig_setmethods__["y"] = setY
    __swig_setmethods__["z"] = setZ
    if _newclass:
        x = property(x,setX)
        y = property(y,setY)
        z = property(z,setZ)
%}
}
%extend pythonapi::Coordinate {
%insert("python") %{
    __swig_getmethods__["x"] = x
    __swig_getmethods__["y"] = y
    __swig_getmethods__["z"] = z
    __swig_setmethods__["x"] = setX
    __swig_setmethods__["y"] = setY
    __swig_setmethods__["z"] = setZ
    if _newclass:
        x = property(x,setX)
        y = property(y,setY)
        z = property(z,setZ)
%}
}

%include "pythonapi_geometry.h"

%include "pythonapi_feature.h"

%include "pythonapi_featureiterator.h"

%include "pythonapi_featurecoverage.h"

%typemap(out) Py_buffer* {
    $result = PyMemoryView_FromBuffer($1);
}

%include "pythonapi_pixeliterator.h"

%include "pythonapi_georeference.h"

%include "pythonapi_rastercoverage.h"

%extend pythonapi::RasterCoverage{
    %insert("python") %{
        def array2raster(self, dataContainer, band=-1):
            self._array2Raster(dataContainer, band)
    %}
}

%extend pythonapi::RasterCoverage{
  %insert("python") %{
      def list2raster(self, dataContainer, band=-1):
          self._list2Raster(dataContainer, band)
  %}
}

%include "pythonapi_catalog.h"
%extend pythonapi::Catalog {
%insert("python") %{
    def __getitem__(self, name):
        return object_cast(self._getitem(name))
%}
}

%include "pythonapi_domain.h"

%include "pythonapi_range.h"

%include "pythonapi_rangeiterator.h"

//%template(NumericRangeIterator) pythonapi::RangeIterator<double, pythonapi::NumericRange, double, Ilwis::NumericRange>;
//%template(ItemRangeIterator) pythonapi::RangeIterator<pythonapi::DomainItem, pythonapi::ItemRange, Ilwis::SPDomainItem, Ilwis::ItemRange>;

%include "pythonapi_datadefinition.h"

%include "pythonapi_columndefinition.h"

%include "pythonapi_domainitem.h"

%include "pythonapi_vertexiterator.h"

// declaring the Const for Python side xUNDEF declarations
%pythoncode %{
        class ReadOnly(type):
          @property
          def sUNDEF(cls):
            return "?"
          @property
          def shUNDEF(cls):
            return 32767
          @property
          def iUNDEF(cls):
            return 2147483645
          @property
          def rUNDEF(cls):
            return -1e+308
          @property
          def flUNDEF(cls):
            return 1e38
          @property
          def i64UNDEF(cls):
            return 9223372036854775808


        class Const(metaclass=ReadOnly):pass
%}

%pythoncode %{
    def do(operation,arg1="",arg2="",arg3="",arg4="",arg5="",arg6="",arg7="",arg8="",arg9="",out=""):
        if str(operation) != "":
            if (type(arg1)==str and len(arg1) > 0):
                arg1 = "'" + arg1 + "'"
            if (type(arg2)==str and len(arg2) > 0):
                arg2 = "'" + arg2 + "'"
            if (type(arg3)==str and len(arg3) > 0):
                arg3 = "'" + arg3 + "'"
            if (type(arg4)==str and len(arg4) > 0):
                arg4 = "'" + arg4 + "'"
            if (type(arg5)==str and len(arg5) > 0):
                arg5 = "'" + arg5 + "'"
            if (type(arg6)==str and len(arg6) > 0):
                arg6 = "'" + arg6 + "'"
            if (type(arg7)==str and len(arg7) > 0):
                arg7 = "'" + arg7 + "'"
            if (type(arg8)==str and len(arg8) > 0):
                arg8 = "'" + arg8 + "'"
            if (type(arg9)==str and len(arg9) > 0):
                arg9 = "'" + arg9 + "'"
            if (type(arg1) == list):
                arg1 = "|".join(map(str,arg1))
            if (type(arg2) == list):
                arg2 = "|".join(map(str,arg2))
            if (type(arg3) == list):
                arg3 = "|".join(map(str,arg3))      
            if (type(arg4) == list):
                arg4 = "|".join(map(str,arg4))  
            if (type(arg5) == list):
                arg5 = "|".join(map(str,arg5))
            if (type(arg6) == list):
                arg6 = "|".join(map(str,arg6))
            if (type(arg7) == list):
                arg7 = "|".join(map(str,arg7)) 
            if (type(arg8) == list):
                arg8 = "|".join(map(str,arg8))
            if (type(arg9) == list):
                arg9 = "|".join(map(str,arg9)) 
            obj = Engine__do(str(out),str(operation),str(arg1),str(arg2),str(arg3),str(arg4),str(arg5),str(arg6),str(arg7),str(arg8),str(arg9))
        else:
            raise IlwisException("no operation given!")
        return object_cast(obj)
%}

%pythoncode %{
     def do2(operation,arg1="",arg2="",arg3="",arg4="",arg5="",arg6="",arg7="",arg8="",arg9="",out=""):
          if str(operation) != "":
              if (type(arg1)==str and len(arg1) > 0):
                  arg1 = "'" + arg1 + "'"
              if (type(arg2)==str and len(arg2) > 0):
                  arg2 = "'" + arg2 + "'"
              if (type(arg3)==str and len(arg3) > 0):
                  arg3 = "'" + arg3 + "'"
              if (type(arg4)==str and len(arg4) > 0):
                  arg4 = "'" + arg4 + "'"
              if (type(arg5)==str and len(arg5) > 0):
                  arg5 = "'" + arg5 + "'"
              if (type(arg6)==str and len(arg6) > 0):
                  arg6 = "'" + arg6 + "'"
              if (type(arg7)==str and len(arg7) > 0):
                  arg7 = "'" + arg7 + "'"
              if (type(arg8)==str and len(arg8) > 0):
                  arg8 = "'" + arg8 + "'"
              if (type(arg9)==str and len(arg9) > 0):
                  arg9 = "'" + arg9 + "'"
              obj = Engine__do2(str(out),str(operation),str(arg1),str(arg2),str(arg3),str(arg4),str(arg5),str(arg6),str(arg7),str(arg8),str(arg9))
          else:
              raise IlwisException("no operation given!")
          return obj
%}

%pythoncode %{
     def version():
         return Engine__version()
%}

%pythoncode %{
    def catalogItems(filter=0):
        return Engine__catalogItems(filter)
%}

%pythoncode %{
   def setWorkingCatalog(path):
       return Engine__setWorkingCatalog(path)
%}

%pythoncode %{
    def setContextProperty(propertyName,path):
        return Engine__setContextProperty(propertyName,path)
%}

%pythoncode %{
def operations(query=""):
    return Engine__operations(query)
%}

%pythoncode %{
def operationMetaData(name, element1 = "syntax", ordinal=-1, element2=""):
  return Engine__operationMetaData(name, element1, ordinal, element2)
%}

%pythoncode %{
   def showProgress(yesno):
       return Engine__showProgress(yesno)
%}

%pythoncode %{
        def removeObject(objid):
            return Engine__removeObject(objid)
%}
%pythoncode %{
    def contextProperty(propertyName):
        return Engine__contextProperty(propertyName)
%}
