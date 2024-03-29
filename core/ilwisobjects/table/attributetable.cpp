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
#include "coverage.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "feature.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "attributetable.h"
#include "logicalexpressionparser.h"
#include "tableselector.h"
#include "mastercatalog.h"

using namespace Ilwis;

AttributeTable::AttributeTable(FeatureCoverage *featureCoverage, int level) : _level(level)
{
    _features.set(featureCoverage);
    setValid(_features.isValid());
}

AttributeTable::AttributeTable(const Resource &res)
{
    quint64 id = mastercatalog()->name2id(res.url().toString(), itFEATURE);
    if ( id != i64UNDEF){
        _features.prepare(id);
        setValid(_features.isValid());
    }
}

IlwisTypes AttributeTable::ilwisType() const
{
    return itATTRIBUTETABLE;
}

quint32 AttributeTable::recordCount() const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _features->featureCount(itFEATURE, _level);
}

quint32 AttributeTable::columnCount() const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _features->attributeDefinitions(_level).definitionCount();
}

Record& AttributeTable::recordRef(quint32 n)
{
    if (!_features.isValid() || n >= recordCount()) {
        throw ErrorObject(TR(QString("attribute table %1 not properly initialized").arg(name())));
    }
    FeatureIterator iter(_features, _level);
    iter = iter + n;
    return (*iter)->recordRef();
}

const Record &AttributeTable::record(quint32 n) const
{
    if (!_features.isValid() || n >= recordCount()) {
        throw ErrorObject(TR(QString("attribute table %1 not properly initialized").arg(name())));
    }
    FeatureIterator iter(_features, _level);
    iter = iter + n;
    return (*iter)->record();
}

ITable AttributeTable::copyTable(const QString &nm)
{
    QString  url;
    if ( nm.indexOf(ANONYMOUS_PREFIX) == 0){
        url = INTERNAL_CATALOG + "/" + Identity::newAnonymousName();
    }else
        url = INTERNAL_CATALOG + "/" + (nm != sUNDEF ? name() : nm);
    IFlatTable tbl;
    tbl.prepare(url);
    for(int i=0; i < columnCount(); ++i){
        tbl->addColumn(columndefinition(i));
    }
    int i = 0;
    for(auto feature : _features){
        const Record& rec = feature->record();
        tbl->record(i++, rec);
    }
    return tbl;
}

void AttributeTable::featureCoverage(FeatureCoverage *f)
{
    _features.set(f);
    setValid(_features.isValid());
}

void AttributeTable::record(quint32 rec, const std::vector<QVariant>& vars, quint32 offset)
{
    if (!_features.isValid() || rec >= recordCount()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    FeatureIterator iter(_features, _level);
    iter = iter + rec;
    (*iter)->record(vars, offset);
}

void AttributeTable::dataLoaded(bool yesno)
{
}

bool AttributeTable::isDataLoaded() const
{
    return true;
}

NumericStatistics AttributeTable::statistics(const QString &columnName, int mode, int bins)
{
    int idx = columnIndex(columnName);
    if ( hasType(columndefinition(idx).datadef().domain()->valueType(),itNUMBER|itDOMAINITEM)){
        std::vector<QVariant> qdata = column(idx);
        std::vector<double> data;
        std::transform(qdata.begin(), qdata.end(), std::back_inserter(data), [](const QVariant& inp){ return inp.toDouble();});

        columndefinitionRef(idx).datadef().statisticsRef().calculate(data.begin(), data.end(), mode, bins);
    }
    return columndefinitionRef(idx).datadef().statistics();
}

bool AttributeTable::canUse(const IlwisObject *obj, bool strict) const
{
    return true;
}

std::vector<QVariant> AttributeTable::column(const QString &columnName) const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return std::vector<QVariant>();
    }
    std::vector<QVariant> values(_features->featureCount(itFEATURE, _level));
    int columnIndex = _features->attributeDefinitions().columndefinition(columnName).id();
    int rec = 0;
    FeatureIterator iter(_features, _level);
    while(iter != iter.end()){
        values[rec++] = (*iter)->recordRef().cell(columnIndex) ;
        ++iter;
    }
    return values;
}

void AttributeTable::column(const QString &columnName, const std::vector<QVariant> &vars, quint32 offset)
{
    quint32 colIndex = columnIndex(columnName);
    column(colIndex,vars, offset);
}

std::vector<QVariant> AttributeTable::column(quint32 index, quint32 start, quint32 stop) const
{
    stop = std::min(stop, recordCount());
    start = std::max((quint32)0, start);
    std::vector<QVariant> data(stop - start);
    FeatureIterator iter(_features, _level);
    iter = iter + start;
    for(quint32 i=start; i < stop; ++i) {
        QVariant v = (*iter)(index);
        data[i - start] = v;
        ++iter;
    }
    return data;
}

void AttributeTable::column(const quint32 columnIndex, const std::vector<QVariant> &vars, quint32 offset)
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    if ( columnIndex == iUNDEF)
        return ;

    FeatureIterator iter(_features, _level);
    iter = iter + offset;
    for(const QVariant& var : vars) {
        if ( iter != iter.end()){
            (*iter)(columnIndex, var);
            ++iter;
        }
    }
}

QVariant AttributeTable::cell(const QString &columnName, quint32 rec, bool asRaw) const
{
    quint32 index = columnIndex(columnName);
    return cell(index, rec, asRaw);
}

QVariant AttributeTable::cell(const quint32 index, quint32 rec, bool asRaw) const
{
    if (!_features.isValid() || rec >= recordCount()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return QVariant();
    }
    FeatureIterator iter(_features, _level);
    iter = iter + rec;
    return (*iter)->cell(index, asRaw);
}

void AttributeTable::recordCount(quint32 r)
{
   // WARN2(ERR_OPERATION_NOTSUPPORTED2,TR("adding columns"), TR("attributes"));
}

void AttributeTable::setCell(const QString &columnName, quint32 rec, const QVariant &var)
{
    quint32 index = columnIndex(columnName);
    setCell(index, rec, var);
}

void AttributeTable::setCell(quint32 col, quint32 rec, const QVariant &var, bool noCheck)
{
    if (!_features.isValid() || rec > recordCount()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ;
    }
    FeatureIterator iter(_features, _level);
    iter = iter + rec;
    (*iter)(col, var);
}

bool AttributeTable::createTable()
{
    return true;
}


ColumnDefinition AttributeTable::columndefinition(const QString &columnName) const
{
    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return ColumnDefinition();
    }
    return _features->attributeDefinitions().columndefinition(columnName);
}

ColumnDefinition AttributeTable::columndefinition(quint32 index) const
{
    return _features->attributeDefinitions().columndefinition(index);
}

ColumnDefinition &AttributeTable::columndefinitionRef(quint32 index)
{
   return _features->attributeDefinitionsRef().columndefinitionRef(index);
}

ColumnDefinition &AttributeTable::columndefinitionRef(const QString &columnName)
{
    return _features->attributeDefinitionsRef().columndefinitionRef(columnName);
}

void AttributeTable::renameColumn(quint32 index, const QString&newName) {
	_features->attributeDefinitionsRef().renameColumn(index, newName);
}

quint32 AttributeTable::columnIndex(const QString &columnName) const
{

    if (!_features.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1,name());
        return iUNDEF;
    }
    return _features->attributeDefinitions().columnIndex(columnName);
}

std::vector<QVariant> AttributeTable::column(const QString &columnName, quint32 start, quint32 stop) const
{
    quint32 index = columnIndex(columnName);
    return column(index, start, stop);
}

bool AttributeTable::addColumn(const ColumnDefinition &def)
{
	return _features->attributeDefinitionsRef().addColumn(def);
}

bool AttributeTable::addColumn(const QString &name, const QString &domainname, const bool readonly)
{
	return _features->attributeDefinitionsRef().addColumn(name, domainname, readonly);
}


bool AttributeTable::addColumn(const QString &nme, const IDomain &domain,const bool readonly)
{
	return _features->attributeDefinitionsRef().addColumn(nme, domain, readonly);
}

void AttributeTable::columndefinition(const ColumnDefinition &def)
{
	_features->attributeDefinitionsRef().columndefinition(def);
}

void AttributeTable::insertRecord(quint32 lowerRec)
{
    WARN2(ERR_OPERATION_NOTSUPPORTED2, TR("inserting records"), TR("attributes"));

}

bool AttributeTable::deleteColumn(const QString &name) {
    WARN2(ERR_OPERATION_NOTSUPPORTED2, TR("deleting columns"), TR("attributes"));
    return false;
}


