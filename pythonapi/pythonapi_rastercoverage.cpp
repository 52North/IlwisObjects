#undef HAVE_IEEEFP_H
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

//#include "numpy/arrayobject.h"

#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisobject.h"

#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/ilwisobjects/domain/datadefinition.h"
#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"

#include "../../core/ilwisobjects/coverage/raster.h"
#include "../../core/ilwisobjects/coverage/coverage.h"

#include "../../core/ilwisobjects/coverage/rastercoverage.h"
#include "../../core/ilwisobjects/coverage/pixeliterator.h"
#include "../../core/ilwisobjects/coverage/geometryhelper.h"

#include "../../core/ilwisobjects/operation/operationoverloads.h"


#include "pythonapi_rastercoverage.h"
#include "pythonapi_engine.h"
#include "pythonapi_error.h"
#include "pythonapi_datadefinition.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_pyobject.h"
#include "pythonapi_geometry.h"
#include "pythonapi_domain.h"

#include <QFloat16>

using namespace pythonapi;

RasterCoverage::RasterCoverage(const Ilwis::IRasterCoverage &coverage):Coverage(Ilwis::ICoverage(coverage)){
}

RasterCoverage::RasterCoverage(){
    Ilwis::IRasterCoverage rc;
    rc.prepare();
    if (rc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(rc));
}

RasterCoverage::RasterCoverage(std::string resource){
    auto input = constructPath(resource);
    Ilwis::IRasterCoverage rc(input, itRASTER);
    if (rc.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(rc));
}

RasterCoverage::~RasterCoverage(){

}

QString RasterCoverage::toId(QString val){ // make the scientific notation of values used in output name "id"-compatible
    return val.replace('-','_').replace('.','_');
}

RasterCoverage* RasterCoverage::operator+(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("add_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarymathraster",
                this->__str__(),
                rc.__str__(),
                "add"
            );
}

RasterCoverage *RasterCoverage::operator+(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("add_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "add"
            );
}

RasterCoverage *RasterCoverage::__radd__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("add_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "add"
            );
}

RasterCoverage *RasterCoverage::operator-(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
        QString("sub_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
        "binarymathraster",
        this->__str__(),
        rc.__str__(),
        "subtract"
    );
}

RasterCoverage *RasterCoverage::operator-(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("sub_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "subtract"
            );
}

RasterCoverage *RasterCoverage::__rsub__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("sub_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "subtract"
            );
}

RasterCoverage *RasterCoverage::operator*(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
        QString("mul_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
        "binarymathraster",
        this->__str__(),
        rc.__str__(),
        "times"
    );
}

RasterCoverage *RasterCoverage::operator*(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("mul_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "times"
            );
}

RasterCoverage *RasterCoverage::__rmul__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("mul_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "times"
            );
}

RasterCoverage *RasterCoverage::__truediv__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
        QString("div_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
        "binarymathraster",
        this->__str__(),
        rc.__str__(),
        "divide"
    );
}

RasterCoverage *RasterCoverage::__truediv__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("div_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "divide"
            );
}

RasterCoverage *RasterCoverage::__rtruediv__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("div_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarymathraster",
                QString("%1").arg(value).toStdString(),
                this->__str__(),
                "divide"
            );
}

RasterCoverage* RasterCoverage::operator>(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("greater_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "greater"
            );
}

RasterCoverage* RasterCoverage::operator<(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("less_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "less"
            );
}

RasterCoverage* RasterCoverage::operator>(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("greater_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "greater"
            );
}

RasterCoverage* RasterCoverage::operator<(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("less_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "less"
            );
}

RasterCoverage* RasterCoverage::operator>=(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("greatereq_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "greatereq"
            );
}

RasterCoverage* RasterCoverage::operator<=(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("lesseq_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "lesseq"
            );
}

RasterCoverage* RasterCoverage::operator>=(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("greatereq_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "greatereq"
            );
}

RasterCoverage* RasterCoverage::operator<=(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("lesseq_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "lesseq"
            );
}

RasterCoverage* RasterCoverage::__and__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("and_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "'and'"
            );
}

RasterCoverage* RasterCoverage::__or__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("or_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "'or'"
            );
}

RasterCoverage* RasterCoverage::__xor__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("xor_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "'xor'"
            );
}

RasterCoverage* RasterCoverage::__rand__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("and_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "'and'"
            );
}

RasterCoverage* RasterCoverage::__ror__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("or_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "'or'"
            );
}

RasterCoverage* RasterCoverage::__rxor__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("xor_%2_%1").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "'xor'"
            );
}

RasterCoverage* RasterCoverage::__and__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("and_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "'and'"
            );
}

RasterCoverage* RasterCoverage::__or__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("or_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "'or'"
            );
}

RasterCoverage* RasterCoverage::__xor__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("xor_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "'xor'"
            );
}

RasterCoverage* RasterCoverage::__eq__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("eq_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "eq"
            );
}

RasterCoverage* RasterCoverage::__ne__(double value){
    return (RasterCoverage*)Engine::_do(
                toId(QString("neq_%1_%2").arg((*this->ptr())->id()).arg(value)).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                QString("%1").arg(value).toStdString(),
                "neq"
            );
}

RasterCoverage* RasterCoverage::__eq__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("eq_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "eq"
            );
}

RasterCoverage* RasterCoverage::__ne__(RasterCoverage &rc){
    return (RasterCoverage*)Engine::_do(
                QString("neq_%1_%2").arg((*this->ptr())->id()).arg((*rc.ptr())->id()).toStdString(),
                "binarylogicalraster",
                this->__str__(),
                rc.__str__(),
                "neq"
            );
}

double RasterCoverage::min(){
    auto raster= this->ptr()->as<Ilwis::RasterCoverage>();
    if ( hasType(raster->datadef().domain()->valueType(), itNUMBER) ){
        raster->statisticsRef(PIXELVALUE);
        return raster->datadefRef().range<Ilwis::NumericRange>()->min();
    }
    return rUNDEF;
}


double RasterCoverage::max(){
    auto raster= this->ptr()->as<Ilwis::RasterCoverage>();
    if ( hasType(raster->datadef().domain()->valueType(), itNUMBER) ){
        raster->statisticsRef(PIXELVALUE);
        return raster->datadefRef().range<Ilwis::NumericRange>()->max();
    }
    return rUNDEF;
}

double RasterCoverage::coord2value(const Coordinate& c){
    return this->ptr()->as<Ilwis::RasterCoverage>()->coord2value(c.data()).value<QVariantMap>()[PIXELVALUE].toDouble();
}

PixelIterator RasterCoverage::__iter__(){
    return PixelIterator(this);
}

template<class V> void setValues(V *buffer, quint64 nItems, Ilwis::PixelIterator& iter){
   for(int i=0; i < nItems; ++i) {
       *iter = buffer[i];
       ++iter;
   }
}
void pythonapi::RasterCoverage::_array2Raster(PyObject* container, int band)
{
#if (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION == 6) /* In 3.6 the PyObject_CheckBuffer() call crashes */
    Py_buffer pybuf;
    int res = PyObject_GetBuffer(container, &pybuf, PyBUF_FORMAT | PyBUF_C_CONTIGUOUS);
    if (res < 0) {
        throw InvalidObject("The object is not a array object.");
    }
#else /* 3.7, 3.8, 3.9 etc */
    if (PyObject_CheckBuffer(container) == 0) {
        throw InvalidObject("The object is not a array object.");
    }
    Py_buffer pybuf;
    PyObject_GetBuffer(container, &pybuf, PyBUF_FORMAT | PyBUF_C_CONTIGUOUS);
#endif

    if (pybuf.buf == NULL){
        throw InvalidObject("The data in the array is empty");
    }
    int nItems = pybuf.len / pybuf.itemsize;

    auto sz = size();
    if ( band == -1){
        if ( sz.linearSize() != nItems){
           throw InvalidObject("mismatch array size and raster size");
        }
    } else {
        if ( sz.xsize() * sz.ysize() != nItems){
            throw InvalidObject("mismatch array size and raster size");
        }
    }

    /*
    np.int8 -> b
    np.byte -> b
    np.uint8 -> B
    np.int16 -> h
    np.uint16 -> H
    np.int32 -> l
    np.uint32 -> L
    np.int64 -> q
    np.uint64 -> Q
    np.float16 -> e
    np.float32 -> f
    np.float64 -> d
    np.float -> d
    np.bool8 -> ?
    np.bool -> ?
    */

    Ilwis::IRasterCoverage raster(this->ptr()->as<Ilwis::RasterCoverage>());
    Ilwis::PixelIterator iter = band != -1 ? raster->band(band) : Ilwis::PixelIterator(raster);
    QString frmt(pybuf.format);
    if (frmt == 'b')
        setValues((qint8*)pybuf.buf, nItems, iter);
    else if (frmt == 'B')
        setValues((quint8*)pybuf.buf, nItems, iter);
    else if (frmt == 'h')
        setValues((qint16*)pybuf.buf, nItems, iter);
    else if (frmt == 'H')
        setValues((quint16*)pybuf.buf, nItems, iter);
    else if (frmt == 'l')
        setValues((qint32*)pybuf.buf, nItems, iter);
    else if (frmt == 'L')
        setValues((quint32*)pybuf.buf, nItems, iter);
    else if (frmt == 'q')
        setValues((qint64*)pybuf.buf, nItems, iter);
    else if (frmt == 'Q')
        setValues((quint64*)pybuf.buf, nItems, iter);
    else if (frmt == 'e') {
        float* buf = new float[nItems];
        qFloatFromFloat16(buf, (qfloat16*)pybuf.buf, nItems);
        setValues(buf, nItems, iter); // more work may be needed here
        delete[] buf;
    }
    else if (frmt == 'f')
        setValues((float*)pybuf.buf, nItems, iter);
    else if (frmt == 'd')
        setValues((double*)pybuf.buf, nItems, iter);
    else if (frmt == '?')
        setValues((bool*)pybuf.buf, nItems, iter);
}

void RasterCoverage::_list2Raster(PyObject *container, int band)
{
    PyObject *iterator = PyObject_GetIter(container);
    PyObject *item;

    if (iterator == NULL) {
       throw InvalidObject("the container is not iterable");
    }

    Ilwis::IRasterCoverage raster(this->ptr()->as<Ilwis::RasterCoverage>());
    Ilwis::PixelIterator iter = band != -1 ? raster->band(band) : Ilwis::PixelIterator(raster);
    double value = 0;
    while ((item = PyIter_Next(iterator))) {
        if (PyLong_Check(item)) {
            value = PyLong_AsLong(item);
        } else if PyFloat_Check(item) {
            value = PyFloat_AsDouble(item);
        }
        *iter = value;
        ++iter;
        Py_DECREF(item);
    }

    Py_DECREF(iterator);
}

double RasterCoverage::pix2value(const PixelD &pix){
    return this->ptr()->as<Ilwis::RasterCoverage>()->pix2value(pix.data());
}

double RasterCoverage::pix2value(const Pixel &pix){
    return this->ptr()->as<Ilwis::RasterCoverage>()->pix2value(pix.data());
}

Size RasterCoverage::size(){
    return Size(this->ptr()->as<Ilwis::RasterCoverage>()->size());
}

void RasterCoverage::setSize(const Size &sz){
    this->ptr()->as<Ilwis::RasterCoverage>()->size(sz.data());
}

void RasterCoverage::unload(){
    this->ptr()->as<Ilwis::RasterCoverage>()->unload();
}

RasterCoverage* RasterCoverage::toRasterCoverage(Object* obj){
    RasterCoverage* ptr = dynamic_cast<RasterCoverage*>(obj);
    if(!ptr)
        throw InvalidObject("cast to RasterCoverage not possible");
    return ptr;
}

CoordinateSystem RasterCoverage::coordinateSystem(){
    return CoordinateSystem(Ilwis::ICoordinateSystem(this->ptr()->as<Ilwis::RasterCoverage>()->georeference()->coordinateSystem()));
}

GeoReference RasterCoverage::geoReference(){
    return GeoReference(Ilwis::IGeoReference(this->ptr()->as<Ilwis::RasterCoverage>()->georeference()));
}

void RasterCoverage::setGeoReference(const GeoReference& gr){
    this->ptr()->as<Ilwis::RasterCoverage>()->georeference(gr.ptr()->as<Ilwis::GeoReference>());
}

DataDefinition& RasterCoverage::datadef() const{
    Ilwis::DataDefinition ilwdef = this->ptr()->as<Ilwis::RasterCoverage>()->datadef();
    DataDefinition* pydef = new DataDefinition(&ilwdef);
    return *pydef;
}

void RasterCoverage::setDataDef(DataDefinition* datdef){
    Ilwis::DataDefinition& ilwdef = this->ptr()->as<Ilwis::RasterCoverage>()->datadefRef();
    ilwdef = datdef->ptr();
}

void RasterCoverage::setDataDef(Domain& dm){
    Ilwis::DataDefinition& ilwdef = this->ptr()->as<Ilwis::RasterCoverage>()->datadefRef();
    DataDefinition* newDef = new DataDefinition(dm);
    ilwdef = newDef->ptr();
}

NumericStatistics* RasterCoverage::statistics(int mode, int bins){
    return new NumericStatistics(this->ptr()->as<Ilwis::RasterCoverage>()->statistics(PIXELVALUE, mode, bins));
 }

PixelIterator RasterCoverage::begin(){
    return PixelIterator(this);
}

PixelIterator RasterCoverage::end(){
    return PixelIterator(this).end();
}

PixelIterator RasterCoverage::band(PyObject *pyTrackIndex){
    QVariant qIndex = resolveIndex(pyTrackIndex);
    if((QMetaType::Type)qIndex.type() == QMetaType::Double){
         Ilwis::PixelIterator* ilwIter = new Ilwis::PixelIterator(this->ptr()->as<Ilwis::RasterCoverage>()->band(qIndex.toDouble()));
        return PixelIterator(ilwIter);
    } else if((QMetaType::Type)qIndex.type() == QMetaType::QString){
         Ilwis::PixelIterator* ilwIter = new Ilwis::PixelIterator(this->ptr()->as<Ilwis::RasterCoverage>()->band(qIndex.toString()));
        return PixelIterator(ilwIter);
    }

    throw InvalidObject("Couldn't find data at this index.");
}

void RasterCoverage::addBand(PyObject* pyTrackIndex, PixelIterator* pyIter){
    QVariant qIndex = resolveIndex(pyTrackIndex);
    if((QMetaType::Type)qIndex.type() == QMetaType::Double){
        if (qIndex.toDouble() != rUNDEF)
            this->_ilwisObject->as<Ilwis::RasterCoverage>()->stackDefinitionRef().insert(qIndex.toDouble());
        this->_ilwisObject->as<Ilwis::RasterCoverage>()->band(qIndex.toDouble(), pyIter->ptr());
    } else if((QMetaType::Type)qIndex.type() == QMetaType::QString){
        if (qIndex.toString() != sUNDEF)
            this->_ilwisObject->as<Ilwis::RasterCoverage>()->stackDefinitionRef().insert(qIndex.toString());
        this->_ilwisObject->as<Ilwis::RasterCoverage>()->band(qIndex.toString(), pyIter->ptr());
    } else {
        throw InvalidObject("The index is not included in the definition.");
    }
}

void RasterCoverage::setBandDefinition(PyObject *pyTrackIndex, const DataDefinition &datdef){
    QVariant qIndex = resolveIndex(pyTrackIndex);
    if((QMetaType::Type)qIndex.type() == QMetaType::Double){
        this->_ilwisObject->as<Ilwis::RasterCoverage>()->setBandDefinition(qIndex.toDouble(), datdef.ptr());
    } else if((QMetaType::Type)qIndex.type() == QMetaType::QString){
        this->_ilwisObject->as<Ilwis::RasterCoverage>()->setBandDefinition(qIndex.toString(), datdef.ptr());
    } else{
        throw InvalidObject("The index is not included in the definition.");
    }
}

QVariant RasterCoverage::resolveIndex(PyObject *pyTrackIndex){
    if(PyFloatCheckExact(pyTrackIndex)){
        double val = PyFloatAsDouble(pyTrackIndex);
        return QVariant(val);
    }else if(PyLongCheckExact(pyTrackIndex)){
        long val = PyLongAsLong(pyTrackIndex);
        return QVariant((double) val);
    }else if(PyUnicodeCheckExact(pyTrackIndex)){
        std::string val = PyBytesAsString(pyTrackIndex);
        return QVariant(QString::fromStdString(val));
    }else if(PyDateTimeCheckExact(pyTrackIndex) || PyDateCheckExact(pyTrackIndex) || PyTimeCheckExact(pyTrackIndex)){
        int year = PyDateTimeGET_YEAR(pyTrackIndex);
        int month = PyDateTimeGET_MONTH(pyTrackIndex);
        int day = PyDateTimeGET_DAY(pyTrackIndex);
        std::string dateStr = std::to_string(year) + dateToString(month) + dateToString(day);
        return QVariant(QString::fromStdString(dateStr));
    }
    throw InvalidObject("Could not resolve index.");
}

std::string RasterCoverage::dateToString(int datepart){
    std::string dateStr;
    if(datepart < 10){
        dateStr = "0" + std::to_string(datepart);
    } else {
        dateStr = std::to_string(datepart);
    }

    return dateStr;
}

void RasterCoverage::setStackDefinition(const Domain& dom, PyObject* items){
    if(PyTupleCheckExact(items)){
        int sz = PyTupleSize(items);
        if(PyFloatCheckExact(PyTupleGetItem(items, 0))){
            std::vector<double> ilwVec;
            for(int i = 0; i < sz; ++i){
                double val = PyFloatAsDouble(PyTupleGetItem(items, i));
                ilwVec.push_back(val);
            }
            this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }else if(PyLongCheckExact(PyTupleGetItem(items, 0))){
            std::vector<double> ilwVec;
            for(int i = 0; i < sz; ++i){
                long val = PyLongAsLong(PyTupleGetItem(items, i));
                ilwVec.push_back((double)val);
            }
            this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }else if(PyUnicodeCheckExact(PyTupleGetItem(items, 0))){
            std::vector<QString> ilwVec;
            for(int i = 0; i < sz; ++i){
                std::string val = PyBytesAsString(PyTupleGetItem(items, i));
                ilwVec.push_back(QString::fromStdString(val));
            }
            this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }else if(PyDateTimeCheckExact(PyTupleGetItem(items, 0)) || PyDateCheckExact(PyTupleGetItem(items, 0)) || PyTimeCheckExact(PyTupleGetItem(items, 0))){
            std::vector<QString> ilwVec;
            for(int i = 0; i < sz; ++i){
                int year = PyDateTimeGET_YEAR(PyTupleGetItem(items, i));
                int month = PyDateTimeGET_MONTH(PyTupleGetItem(items, i));
                int day = PyDateTimeGET_DAY(PyTupleGetItem(items, i));
                std::string dateStr = std::to_string(year) + dateToString(month) + dateToString(day);
                ilwVec.push_back(QString::fromStdString(dateStr));
            }
            this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().setSubDefinition(dom.ptr()->as<Ilwis::Domain>(), ilwVec);
        }
    }
}

quint32 RasterCoverage::indexOf(const std::string& variantId) const{
    return this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().index(QString::fromStdString(variantId));
}

quint32 RasterCoverage::indexOf(double domainItem) const{
    return this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().index(domainItem);
}

quint32 RasterCoverage::indexOf(PyObject* obj) const{
    if(PyDateTimeCheckExact(obj) || PyDateCheckExact(obj) || PyTimeCheckExact(obj)){
        int year = PyDateTimeGET_YEAR(obj);
        int month = PyDateTimeGET_MONTH(obj);
        int day = PyDateTimeGET_DAY(obj);
        std::string dateStr = std::to_string(year) + std::to_string(month) + std::to_string(day);
        return this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().index(QString::fromStdString(dateStr));
    }
    return iUNDEF;
}

std::string RasterCoverage::atIndex(quint32 idx) const{
    QString qStr =  this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().index(idx);
    return qStr.toStdString();
}

PyObject* RasterCoverage::indexes() const{
    std::vector<QString> qVec = this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().indexes();
    PyObject* pyTup = newPyTuple(qVec.size());

    for(int i = 0; i < qVec.size(); ++i){
        std::string actStr = qVec[i].toStdString();
        setTupleItem(pyTup, i, PyBuildString(actStr));
    }

    return pyTup;
}

quint32 RasterCoverage::countStackDomainItems() const{
    return this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().count();
}

Domain RasterCoverage::stackDomain() const{
    Ilwis::IDomain ilwDom =  this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().domain();
    return Domain(Ilwis::IDomain(ilwDom));
}

void RasterCoverage::clear(){
    this->ptr()->as<Ilwis::RasterCoverage>()->stackDefinitionRef().clearSubFeatureDefinitions();
}

RasterCoverage RasterCoverage::select(std::string geomWkt){
    Geometry geom (geomWkt, coordinateSystem());
    if (geom.__bool__())
        return select(geom);
    else
        return RasterCoverage();
}

RasterCoverage RasterCoverage::select(Geometry& geom){
    const geos::geom::Envelope *env = geom.ptr()->getEnvelopeInternal();
    Ilwis::Envelope envelope(Ilwis::Coordinate(env->getMinX(), env->getMinY()),Ilwis::Coordinate(env->getMaxX(), env->getMaxY()));
    Ilwis::BoundingBox box = this->ptr()->as<Ilwis::RasterCoverage>()->georeference()->coord2Pixel(envelope);
    box.min_corner().x = std::max(box.min_corner().x, 0);
    box.min_corner().y = std::max(box.min_corner().y, 0);
    box.max_corner().x = std::min(box.max_corner().x, (int)(size().xsize() - 1));
    box.max_corner().y = std::min(box.max_corner().y, (int)(size().ysize() - 1));
    envelope = this->ptr()->as<Ilwis::RasterCoverage>()->georeference()->pixel2Coord(box);
    QString grfcode = QString("code=georef:type=corners,cornerofcorners=yes,csy=%1,envelope=%2,gridsize=%3").arg(this->ptr()->as<Ilwis::RasterCoverage>()->coordinateSystem()->id()).arg(envelope.toString()).arg(box.size().toString());
    Ilwis::IGeoReference grf(grfcode);

    Ilwis::IRasterCoverage map2;
    map2.prepare();

    map2->coordinateSystem(this->ptr()->as<Ilwis::RasterCoverage>()->coordinateSystem());
    map2->georeference(grf);
    map2->datadefRef() = this->ptr()->as<Ilwis::RasterCoverage>()->datadef();

    // prefill all pixels with nodata
    Ilwis::PixelIterator iterOut(map2);
    Ilwis::PixelIterator iterOutEnd = iterOut.end();
    while(iterOut != iterOutEnd) {
        *iterOut = rUNDEF;
        ++iterOut;
    }

    // now fill with clip
    Ilwis::PixelIterator iterIn(this->ptr()->as<Ilwis::RasterCoverage>(), geom.ptr().get());
    iterOut = Ilwis::PixelIterator(map2, geom.ptr().get());
    Ilwis::PixelIterator iterInEnd = iterIn.end();
    while( iterIn != iterInEnd && iterOut != iterOutEnd) {
        *iterOut = *iterIn;
        ++iterOut;
        ++iterIn;
    }

    return RasterCoverage(map2);
}

RasterCoverage* RasterCoverage::reprojectRaster(std::string newName, quint32 epsg, std::string interpol){
    CoordinateSystem targetPyCsy ("code=epsg:" + std::to_string(epsg));
    Ilwis::ICoordinateSystem targetIlwCsy = targetPyCsy.ptr()->as<Ilwis::CoordinateSystem>();
    Ilwis::IGeoReference georef = this->geoReference().ptr()->as<Ilwis::GeoReference>();
    Ilwis::ICoordinateSystem sourceCsy = georef->coordinateSystem();
    if(sourceCsy->name() == "unknown"){
        this->geoReference().setCoordinateSystem(targetPyCsy);
        return this;
    }
    Ilwis::Envelope env  = this->ptr()->as<Ilwis::RasterCoverage>()->envelope();
    env = sourceCsy->convertEnvelope(targetIlwCsy, env);
    Ilwis::BoundingBox bo = georef->coord2Pixel(env);
    Ilwis::Size<> sz = bo.size();
    std::string refStr = "code=georef:type=corners,csy=epsg:" + std::to_string(epsg) + ",envelope=" +
            env.toString().toStdString() + ",gridsize=" + std::to_string(sz.xsize()) + " " + std::to_string(sz.ysize()) +
            ",name=grf1";

    GeoReference grf(refStr);
    QString expr = QString::fromStdString(newName + "=resample(" + this->name() + ",grf1," + interpol  + ")");
    //expr = expr.fromStdString(newName + "=resample(" + this->name() + ",grf1," + interpol  + ")");
    Ilwis::ExecutionContext ctx;
    Ilwis::SymbolTable syms;
    Ilwis::commandhandler()->execute(expr,&ctx,syms);
    QString path;
    path = path.fromStdString("ilwis://internalcatalog/" + newName);
    Ilwis::IRasterCoverage raster (path);
    return new RasterCoverage(raster);
}

RasterCoverage* RasterCoverage::clone(){
    Ilwis::IRasterCoverage ilwRc = this->ptr()->as<Ilwis::RasterCoverage>()->clone();
    return new RasterCoverage(ilwRc);
}

Envelope RasterCoverage::envelope(){
    return Envelope(this->ptr()->as<Ilwis::RasterCoverage>()->envelope());
}

const QString RasterCoverage::getStoreFormat() const {
    return "map";
}
