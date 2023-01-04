#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/ilwisobjects/ilwisobject.h"

#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/ilwisobjects/domain/datadefinition.h"
#include "../../core/ilwisobjects/table/columndefinition.h"
#include "../../core/ilwisobjects/table/table.h"
#include "../../core/ilwisobjects/table/basetable.h"

#include "pythonapi_table.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_feature.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_error.h"
#include "pythonapi_columndefinition.h"

namespace pythonapi {

    Table::Table(){
        Ilwis::ITable t;
        t.prepare();
        if (t.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(t));
    }

    Table::Table(const Ilwis::ITable & table):IlwisObject(new Ilwis::IIlwisObject(table)){
    }

    Table::Table(std::string resource, const IOOptions& opt){
        auto input = constructPath(resource);
        Ilwis::ITable t(input, itTABLE, opt.ptr());
        if (t.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(t));
    }

    quint32 Table::recordCount() const{
        return this->ptr()->as<Ilwis::Table>()->recordCount();
    }

    quint32 Table::columnCount() const{
        return this->ptr()->as<Ilwis::Table>()->columnCount();
    }

    void Table::addColumn(const std::string& name, const std::string& domain) {
        if (!this->ptr()->as<Ilwis::Table>()->addColumn(QString::fromStdString(name),QString::fromStdString(domain)))
            throw Ilwis::ErrorObject(QString("Could not add column '%1' of domain '%2' to the list of columns").arg(QString::fromStdString(name)).arg(QString::fromStdString(domain)));
    }

    void Table::addColumn(ColumnDefinition& coldef){
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        if (!this->ptr()->as<Ilwis::Table>()->addColumn(*ilwDef))
            throw Ilwis::ErrorObject(QString("Could not add column '%1' of domain '%2' to the list of columns").arg(ilwDef->name()).arg(ilwDef->datadef().domain()->name()));
    }

    PyObject* Table::columns() const{
        Ilwis::ITable tbl = this->ptr()->as<Ilwis::Table>();
        PyObject* list = newPyTuple(tbl->columnCount());
        for(int i = 0; i < tbl->columnCount(); i++){
            Ilwis::ColumnDefinition coldef = tbl->columndefinition(i);
            if (!setTupleItem(list, i, PyUnicodeFromString(coldef.name().toStdString().data())))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(tbl->columndefinition(i).name()));
        }
        return list;
    }

    PyObject* Table::select(const std::string& conditions) const{
        std::vector<quint32> lst = this->ptr()->as<Ilwis::Table>()->select(QString::fromStdString(conditions));
        PyObject* list = newPyTuple(lst.size());
        int i = 0;
        for(auto it = lst.begin(); it != lst.end(); it++){
            if (!setTupleItem(list, i++, PyLongFromSize_t(*it)))
                throw Ilwis::ErrorObject(QString("internal conversion error while trying to add '%1' to list of columns").arg(*it));
        }
        return list;
    }

    qint32 Table::columnIndex(const std::string& name) const{
        quint32 idx = this->ptr()->as<Ilwis::Table>()->columnIndex(QString::fromStdString(name));
        if (idx == Ilwis::iUNDEF)
            return Ilwis::iUNDEF;
        else
            return idx;
    }

    PyObject* Table::cell(const std::string& name, quint32 rec){
        QVariant ret = this->ptr()->as<Ilwis::Table>()->cell(QString::fromStdString(name), rec,false);
        if (!ret.isValid())
            throw std::out_of_range(QString("No attribute '%1' found or record %2 out of bound").arg(name.c_str()).arg(rec).toStdString());
        return QVariant2PyObject(ret);
    }

    PyObject* Table::cell(quint32 colIndex, quint32 rec){
         if(!this->ptr()->as<Ilwis::Table>()->isDataLoaded()){
             this->ptr()->as<Ilwis::Table>()->loadData();
        }
        if(this->ptr()->as<Ilwis::Table>()->isDataLoaded()){
            QVariant ret = this->ptr()->as<Ilwis::Table>()->cell(colIndex, rec,false);
            if (!ret.isValid())
                throw std::out_of_range(QString("No attribute in '%1.' column found or record %2 out of bound").arg(colIndex).arg(rec).toStdString());
            return QVariant2PyObject(ret);
        } else {
            throw InvalidObject("Data of the table is not loaded. Access a feature first.");
        }
    }

    void Table::setCell(const std::string& name, quint32 rec, const PyObject* value){
        QVariant* v = PyObject2QVariant(value);
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, *v);
        delete v;
    }

    void Table::setCell(quint32 colIndex, quint32 rec, const PyObject* value){
        QVariant* v = PyObject2QVariant(value);
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, *v);
        delete v;
    }

    void Table::setCell(const std::string& name, quint32 rec, qint64 value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, qint64 value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    void Table::setCell(const std::string& name, quint32 rec, std::string value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value.c_str()));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, std::string value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value.c_str()));
    }

    void Table::setCell(const std::string& name, quint32 rec, double value){
        this->ptr()->as<Ilwis::Table>()->setCell(QString::fromStdString(name), rec, QVariant(value));
    }

    void Table::setCell(quint32 colIndex, quint32 rec, double value){
        this->ptr()->as<Ilwis::Table>()->setCell(colIndex, rec, QVariant(value));
    }

    PyObject* Table::column(const std::string& name) const{
        return StdVectorOfQVariant2PyTuple(this->ptr()->as<Ilwis::Table>()->column(QString::fromStdString(name)));
    }

    PyObject* Table::column(quint32 columnIndex) const{
        return StdVectorOfQVariant2PyTuple(this->ptr()->as<Ilwis::Table>()->column(columnIndex));
    }

    void Table::setCells(const std::string &name, PyObject *container)
    {
        if ( PyList_Check(container)){
            PyObject *iterator = PyObject_GetIter(container);
            PyObject *item;

            if (iterator == NULL) {
               throw InvalidObject("the container is not iterable");
            }

            int colIndex = this->ptr()->as<Ilwis::Table>()->columnIndex(QString::fromStdString(name));
            if ( colIndex == iUNDEF){
                throw InvalidObject("The column is uknonwn.");
            }
            std::vector<QVariant> values;

            while ((item = PyIter_Next(iterator))) {
                QVariant value;
                if (PyLong_Check(item)) {
                    value = (qint64) PyLong_AsLong(item);
                } else if PyFloat_Check(item) {
                    value = PyFloat_AsDouble(item);
                } else {
                    if ( item){
                        if( PyUnicode_Check(item) ){
                            auto s = PyUnicode_AsUTF8String(item);
                            std::string result = PyBytes_AsString(s) ;
                            value = QString::fromStdString(result);
                            Py_XDECREF(s);
                        }
                    }
                }
                values.push_back(value);
                Py_DECREF(item);
            }

            Py_DECREF(iterator);
            this->ptr()->as<Ilwis::Table>()->column(colIndex, values);
            return;
        }else {
            Py_buffer pybuf;
            PyObject_GetBuffer(container, &pybuf, PyBUF_FORMAT | PyBUF_C_CONTIGUOUS);
            if ( pybuf.buf == NULL){
                throw InvalidObject("the container is not a buffer object");
            }
            int len = pybuf.len;
            int colIndex = this->ptr()->as<Ilwis::Table>()->columnIndex(QString::fromStdString(name));
            if ( colIndex == iUNDEF){
                throw InvalidObject("The column is uknonwn.");
            }
            IlwisTypes valueType = IlwisObject::determineBufferFormat(pybuf);

            for(int i=0; i < len; ++i){
                if ( hasType(valueType, itNUMBER)){
                    double v;
                    if ( valueType == itDOUBLE)
                        v = ((double *)pybuf.buf)[i];
                    if ( valueType == itINT64){
                        v = ((qint64 *)pybuf.buf)[i];
                    }
                    if ( valueType == itINT32){
                        v = ((qint32 *)pybuf.buf)[i];
                    }
                    if ( valueType == itINT8){
                        v = ((char *)pybuf.buf)[i];
                    }
                    this->ptr()->as<Ilwis::Table>()->setCell(colIndex, i, v);
                }else if ( hasType(valueType, itDOMAINITEM)){
                    QString s;
                    s = ((char *)pybuf.buf)[i];
                    this->ptr()->as<Ilwis::Table>()->setCell(colIndex, i, s);
                }
            }
        }
    }

    PyObject* Table::record(quint32 rec) const{
        return StdVectorOfQVariant2PyTuple(this->ptr()->as<Ilwis::Table>()->record(rec));
    }

    Table* Table::toTable(Object *obj){
        Table* ptr = dynamic_cast<Table*>(obj);
        if(!ptr)
            throw InvalidObject("cast to Table not possible");
        return ptr;
    }

    ColumnDefinition Table::columnDefinition(const std::string& name) const{
        QString qName;
        qName = qName.fromStdString(name);
        Ilwis::ColumnDefinition* coldef = new Ilwis::ColumnDefinition(this->ptr()->as<Ilwis::Table>()->columndefinition(qName));
        return ColumnDefinition(coldef);
    }

    ColumnDefinition Table::columnDefinition(quint32 index) const{
        Ilwis::ColumnDefinition* coldef = new Ilwis::ColumnDefinition(this->ptr()->as<Ilwis::Table>()->columndefinition(index));
        return ColumnDefinition(coldef);
    }

    void Table::setColumnDefinition(ColumnDefinition& coldef){
        Ilwis::ColumnDefinition* ilwDef = coldef.ptr().get();
        this->ptr()->as<Ilwis::Table>()->columndefinition(*ilwDef);
    }

    void Table::setColumnDefinition(const std::string &name, ColumnDefinition* coldef){
        QString qName;
        qName = qName.fromStdString(name);
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(qName);
        Ilwis::ColumnDefinition* newDef = coldef->ptr().get();
        ilwdef = Ilwis::ColumnDefinition(*newDef, ilwdef.columnindex());
    }

    void Table::setColumnDefinition(quint32 index, ColumnDefinition *coldef){
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(index);
        Ilwis::ColumnDefinition* newDef = coldef->ptr().get();
        ilwdef = Ilwis::ColumnDefinition(*newDef, ilwdef.columnindex());
    }

    NumericStatistics *Table::statistics(const std::string& attr, int mode, int bins)
    {
        auto *stats =  new NumericStatistics(this->ptr()->as<Ilwis::Table>()->statistics(QString::fromStdString(attr), mode, bins));
        Ilwis::ColumnDefinition& ilwdef = this->ptr()->as<Ilwis::Table>()->columndefinitionRef(QString::fromStdString(attr));
        if ( hasType(ilwdef.datadef().domain()->valueType(), itNUMBER)){

            double rMin = stats->prop(pythonapi::PropertySets::pMIN);
            double rMax = stats->prop(pythonapi::PropertySets::pMIN);
            ilwdef.datadef().range()->as<Ilwis::NumericRange>()->min(rMin);
            ilwdef.datadef().range()->as<Ilwis::NumericRange>()->min(rMax);

         }
        return stats;
    }

    ColumnDefinition Table::__getitem__(quint32 index)
    {
        Ilwis::ColumnDefinition ilwDef = this->ptr()->as<Ilwis::Table>()->columndefinition(index);
        return ColumnDefinition(new Ilwis::ColumnDefinition(ilwDef));
    }

    const QString Table::getStoreFormat() const {
        return "table";
    }

} // namespace pythonapi
