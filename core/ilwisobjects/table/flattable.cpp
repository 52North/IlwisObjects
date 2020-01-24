/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "domainitem.h"
#include "selectabletable.h"
#include "basetable.h"
#include "flattable.h"
#include "tablemerger.h"
#include "itemrange.h"

using namespace Ilwis;

FlatTable::FlatTable()
{
}

FlatTable::FlatTable(const Resource& resource) : BaseTable(resource){

}

FlatTable::~FlatTable()
{
    _datagrid.clear();
}

bool FlatTable::createTable()
{
    if(!BaseTable::createTable()) {
        return false;
    }
    if (_datagrid.size() != recordCount())
        _datagrid.resize(recordCount());

    for(unsigned int i=0; i < recordCount(); ++i) {
        Record rec(std::vector<QVariant>(_attributeDefinition.definitionCount()));
        _datagrid[i] = rec;
    }
    return true;
}

Record& FlatTable::newRecord()
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("could not load %1")).arg(name()));
    }

    std::vector<QVariant> values;
    initRecord(values);
    record(NEW_RECORD, values);

    return _datagrid.back();
}

void FlatTable::removeRecord(quint32 rec)
{
    if ( rec < recordCount()){
        _datagrid.erase(_datagrid.begin() + rec);
        BaseTable::removeRecord(rec);
    }
}

void FlatTable::insertRecord(quint32 lowerRec)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("could not load %1")).arg(name()));
    }

    if (isReadOnly()) {
        return;
    }

    changed(true);
    if (lowerRec < _datagrid.size()) {
        _datagrid.insert( _datagrid.begin() + lowerRec, std::vector<QVariant>(_attributeDefinition.definitionCount()));
        recordCount(std::max(recordCount(), (quint32)_datagrid.size()));
        std::vector<QVariant> values;
        initRecord(values);
        record(lowerRec, values);
    }
 
}

bool FlatTable::prepare(const IOOptions& options)
{
    return Table::prepare(options);
}

bool FlatTable::isValid() const
{
    return BaseTable::isValid();
}

bool FlatTable::deleteColumn(const QString &name) {
    int cIndex  = columnIndex(name);
    if (cIndex != iUNDEF) {
        if (BaseTable::deleteColumn(name)) {
            _attributeDefinition.deleteColumn(name);
            for (int i = 0; i < _datagrid.size(); ++i) {
                Record& rec = recordRef(i);
                rec.deleteColumn(cIndex);

            }
            return true;
        }
    }
    return false;
}

bool FlatTable::addColumn(const QString &name, const IDomain &domain,const bool readonly)
{
    bool ok = BaseTable::addColumn(name, domain, readonly);
    if(!ok) {
        return false;
    }
    if ( isDataLoaded()){
        for(Record& row : _datagrid) {
            row.addColumn();
            row.changed(true);
        }
        initValuesColumn(name);
    }
    return true;
}

bool FlatTable::addColumn(const ColumnDefinition &def)
{
    bool ok = BaseTable::addColumn(def);
    if(!ok) {
        return false;
    }
    if (  isDataLoaded()) {
        for(Record& row : _datagrid) {
            row.addColumn();
            row.changed(true);
        }
        initValuesColumn(def.name());
    }
    return true;
}

bool FlatTable::addColumn(const QString &name, const QString& domainname,const bool readonly)
{
    bool ok = BaseTable::addColumn(name, domainname, readonly);
    if(!ok) {
        return false;
    }
    if ( isDataLoaded()){
        for(Record& row : _datagrid) {
            row.addColumn();
            row.changed(true);
        }
        initValuesColumn(name);
    }
    return true;
}

std::vector<QVariant> FlatTable::column(quint32 index, quint32 start, quint32 stop) const {
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return std::vector<QVariant>();
    }

    if ( !isColumnIndexValid(index)) {
        return std::vector<QVariant>();
    }

    stop = std::min(stop, recordCount());
    start = std::max((quint32)0, start);
    std::vector<QVariant> data(stop - start);
    for(quint32 i=start; i < stop; ++i) {
        data[i - start] = _datagrid[i].cell(index);
    }
    return data;
}

std::vector<QVariant> FlatTable::column(const QString &nme, quint32 start, quint32 stop) const
{
    quint32 index = columnIndex(nme);
    return column(index, start, stop);

}

void FlatTable::column(quint32 index, const std::vector<QVariant> &vars, quint32 offset)
{
    if (index >= columnCount()) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"Column index", name());
        return ;
    }
    column( _attributeDefinition[index].name(), vars, offset);
}

void FlatTable::column(const QString &nme, const std::vector<QVariant> &vars, quint32 offset)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);

    quint32 index = columnIndex(nme);
    if ( !isColumnIndexValid(index)) {
        return ;
    }

    quint32 rec = offset;
    _attributeDefinition[index].changed(true);
    for(const QVariant& var : vars) {
        if ( rec < _datagrid.size()){
            _datagrid[rec].changed(true);
            _datagrid[rec++].cell(index, checkInput(var, index));
        }
        else {
            _datagrid.push_back(std::vector<QVariant>(_attributeDefinition.definitionCount()));
            _datagrid[rec].changed(true);
            _datagrid[rec++].cell(index,checkInput(var,index));
            BaseTable::recordCount((quint32)_datagrid.size());
        }
    }

}

const Record& FlatTable::record(quint32 rec) const{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("failed load of table %1")).arg(name()));
    }

    if ( rec < recordCount() && _datagrid.size() != 0) {
         return _datagrid[rec];
    }
    throw ErrorObject(QString("Requested record number is not in the table").arg(rec));
}

Record& FlatTable::recordRef(quint32 rec)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        throw ErrorObject(QString(TR("failed load of table %1")).arg(name()));
    }

    if ( rec < recordCount() && _datagrid.size() != 0) {
         return _datagrid[rec];
    }
    throw ErrorObject(QString("Requested record number is not in the table").arg(rec));
}

void FlatTable::record(quint32 rec, const std::vector<QVariant>& vars, quint32 offset)
{

    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);
    if ( rec >=_datagrid.size() ) {
        _datagrid.push_back(std::vector<QVariant>(_attributeDefinition.definitionCount()));
        _datagrid.back().changed(true);
        // datagrid and recordcount might not have the same size as recordcount is the
        // number of records the metadata says that should be there and datagrid contains the actual
        // data that is loaded
        recordCount(std::max(recordCount(), (quint32)_datagrid.size()));
        rec = (quint32)_datagrid.size() - 1;
    }
    quint32 col = offset;
    int cols = std::min((quint32)vars.size() - offset, columnCount());
    for(const QVariant& var : vars) {
        if ( col < cols){
            _datagrid[rec].changed(true);
            _datagrid[rec].cell(col, checkInput(var, col));
            ++col;
        }
    }

}

QVariant FlatTable::cell(const QString& col, quint32 rec, bool asRaw) const {
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return QVariant();
    }
    quint32 index = columnIndex(col);
    return cell(index , rec, asRaw);
}

QVariant FlatTable::cell(const quint32 index, quint32 rec, bool asRaw) const
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return QVariant();
    }

    if ( !isColumnIndexValid(index)) {
        return QVariant();
    }
    if ( rec < recordCount()) {
        QVariant var = _datagrid[rec].cell(index);
        if ( !asRaw) {
            ColumnDefinition coldef = columndefinition(index);
            return coldef.datadef().domain<>()->impliedValue(var);
        }
        return var;
    }
    kernel()->issues()->log(TR(ERR_INVALID_RECORD_SIZE_IN).arg(name()),IssueObject::itWarning);
    return QVariant();
}

void  FlatTable::setCell(quint32 index, quint32 rec, const QVariant& var, bool noCheck){
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( !isColumnIndexValid(index)) {
        return;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);

    _attributeDefinition[index].changed(true);

    if ( rec >= _datagrid.size()) {
        newRecord();
        rec = (quint32)_datagrid.size() - 1;
    }
	if (noCheck) {
		_datagrid[rec].cell(index, var);
		_attributeDefinition[index].datadef().range()->add(var);
	}
	else
		_datagrid[rec].cell(index, checkInput(var, index));
    _datagrid[rec].changed(true);

}

void FlatTable::setCell(const QString &col, quint32 rec, const QVariant &var)
{
    if (!const_cast<FlatTable *>(this)->initLoad()) {
        return ;
    }

    if ( isReadOnly()) {
        return ;
    }
    changed(true);

    quint32 index = columnIndex(col);
    setCell(index, rec, var);

}

IlwisTypes FlatTable::ilwisType() const
{
    return itFLATTABLE;
}

IlwisObject *FlatTable::clone()
{
    if (initLoad()){
        FlatTable *tbl = new FlatTable();
        copyTo(tbl);
        tbl->_datagrid = _datagrid;
        for(auto& record : tbl->_datagrid){
            record.changed(false);
        }
        return tbl;
    }
    return 0;
}
ITable FlatTable::copyTable(const QString &nm)
{
    QString  url;
    if ( nm.indexOf(ANONYMOUS_PREFIX) == 0){
        url = INTERNAL_CATALOG + "/" + Identity::newAnonymousName();
    }else if ( nm.indexOf("://") != -1){
        url = nm;
    }else if ( nm != "" && nm != sUNDEF)
        url = INTERNAL_CATALOG + "/" + (nm != sUNDEF ? name() : nm);

    IFlatTable tbl;
    if (initLoad()){
        if ( url != "")
            tbl.prepare(url);
        else
            tbl.prepare();
        for(int i=0; i < columnCount(); ++i){
            tbl->addColumn(columndefinition(i));
        }
        int i = 0;
        for(auto record : _datagrid){
            tbl->record(i++, record);
        }
    }
    return tbl;
}

bool FlatTable::merge(const IlwisObject *obj)
{
    return BaseTable::merge(obj);
}

bool FlatTable::canUse(const IlwisObject *obj, bool strict) const
{
    return true;
}

void FlatTable::copyTo(IlwisObject *obj){
    BaseTable::copyTo(obj);
}

bool FlatTable::initLoad(){
    if ( isDataLoaded()) {
        return true;
    }

    bool ok = BaseTable::initLoad();

    for(int i=0; ok && i < columnCount() && _datagrid.size() > 0; ++i){
        QVariant var = cell(i,0);
        if ( !var.isValid()) {
            initValuesColumn(columndefinition(i).name());
        }
    }
    return ok;
}

void FlatTable::recordCount(quint32 r)
{
    quint32 old_r = BaseTable::recordCount();
    BaseTable::recordCount(r);
	_datagrid.resize(r);        // add records, with zero columns
	if (BaseTable::columnCount() > 0)
		for (int i = old_r; i < r; ++i) // add empty columns, for the new reords
			_datagrid[i] = std::vector<QVariant>(BaseTable::columnCount());
}

quint32 Ilwis::FlatTable::recordCount() const
{
    return BaseTable::recordCount();
}

void FlatTable::recalcRanges() {
	for (int c = 0; c < columnCount(); ++c) {
		if (hasType(columndefinitionRef(c).datadef().domain()->ilwisType(), itNUMERICDOMAIN)) {
			double rmin = 1e308;
			double rmax = -1e308;
			for (int r = 0; r <recordCount(); ++r) {
				double v = cell(c, r).toDouble();
				rmin = std::min(v, rmin);
				rmax = std::max(v, rmax);
			}
			IlwisTypes vt = columndefinitionRef(c).datadef().domain()->valueType();
			columndefinitionRef(c).datadef().range(new NumericRange(rmin, rmax, hasType(vt, itINTEGER) ? 1 : 0));
		}
	}
}
