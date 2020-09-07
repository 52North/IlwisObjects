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
#include "range.h"
#include "connectorinterface.h"
#include "itemdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "datadefinition.h"


using namespace Ilwis;

AttributeDefinition::AttributeDefinition()
{
}

bool AttributeDefinition::addColumn(const ColumnDefinition& def){

    if ( _columnDefinitionsByName.find(def.name()) != _columnDefinitionsByName.end()) { // no duplicates
        return false;
    }
    ColumnDefinition coldef = def;
    coldef.columnindex(_columnDefinitionsByIndex.size());
    _columnDefinitionsByName[coldef.name()] = (quint32)_columnDefinitionsByIndex.size();
    _columnDefinitionsByIndex.push_back(coldef);

    return true;
}

void AttributeDefinition::renameColumn(quint32 index, const QString&newName) {

	if (index < _columnDefinitionsByIndex.size()) {
		QString name = _columnDefinitionsByIndex[index].name();
		_columnDefinitionsByIndex[index].name(newName);
		auto iter = _columnDefinitionsByName.find(name);
		if (iter != _columnDefinitionsByName.end()) {
			_columnDefinitionsByName.erase(iter);
			_columnDefinitionsByName[newName] = index;
		}
	}
}

void AttributeDefinition::deleteColumn(const QString& name) {
    bool found = false;
    for (int i = 0; i < _columnDefinitionsByIndex.size(); ++i) {
        if (_columnDefinitionsByIndex[i].name() != name) {
            _columnDefinitionsByIndex[found ? i-1 : i] = _columnDefinitionsByIndex[i];
        }
        else
            found = true;
   }
   if (found) {
       _columnDefinitionsByIndex.resize(_columnDefinitionsByIndex.size() - 1);
	   _columnDefinitionsByName.clear();
	   for (int i = 0; i < _columnDefinitionsByIndex.size(); ++i) {
		   _columnDefinitionsByName[_columnDefinitionsByIndex[i].name()] = i;
	   }
   }

}

bool AttributeDefinition::addColumn(const QString &name, const QString &domainname, bool readonly)
{
    IDomain dom;
    if(!dom.prepare(domainname))
        return false;
    return addColumn({name, dom, (quint32)_columnDefinitionsByIndex.size(), readonly});
}


bool AttributeDefinition::addColumn(const QString &name, const IDomain &domain, bool readonly)
{
    return addColumn({name, domain, (quint32)_columnDefinitionsByIndex.size(), readonly});
}

ColumnDefinition AttributeDefinition::columndefinition(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter != _columnDefinitionsByName.end()) {
       ColumnDefinition coldef = _columnDefinitionsByIndex[(*iter).second];
       return coldef;
    }
    return ColumnDefinition();
}

ColumnDefinition AttributeDefinition::columndefinition(quint32 index) const
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    return ColumnDefinition();
}

ColumnDefinition &AttributeDefinition::columndefinitionRef(quint32 index)
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    throw ErrorObject(TR(QString("Invalid column index for %1 used").arg(index)));
}

const ColumnDefinition &AttributeDefinition::columndefinitionRef(quint32 index) const
{
    if ( index < _columnDefinitionsByIndex.size())
        return _columnDefinitionsByIndex[index];
    throw ErrorObject(TR(QString("Invalid column index for %1 used").arg(index)));
}

ColumnDefinition &AttributeDefinition::columndefinitionRef(const QString &columnname)
{
    auto iter = _columnDefinitionsByName.find(columnname);
    if ( iter != _columnDefinitionsByName.end()) {
       return _columnDefinitionsByIndex[(*iter).second];
    }
    throw ErrorObject(TR(QString("Invalid column name %1 used").arg(columnname)));
}

void AttributeDefinition::columndefinition(const ColumnDefinition &coldef)
{
    if ( coldef.id() >=  _columnDefinitionsByIndex.size())     {
        addColumn({coldef.name(), coldef.datadef().domain<>()});
    } else {
        auto iter1 = _columnDefinitionsByName.find(coldef.name());
        if ( iter1 != _columnDefinitionsByName.end()) {
            ColumnDefinition cdef = coldef;
            cdef.columnindex((*iter1).second);
            _columnDefinitionsByIndex[cdef.columnindex()] = cdef;
            (*iter1).second = cdef.columnindex();

        }else {
            addColumn(coldef);
        }
    }
}

quint32 AttributeDefinition::columnIndex(const QString &nme) const
{
    auto iter = _columnDefinitionsByName.find(nme);
    if ( iter == _columnDefinitionsByName.end()) {
        //kernel()->issues()->log(TR(ERR_COLUMN_MISSING_2).arg(nme).arg(name()),IssueObject::itWarning);
        return iUNDEF;
    }
    return (*iter).second;
}

quint32 AttributeDefinition::columnCount() const
{
    return (quint32)_columnDefinitionsByIndex.size();
}

ColumnDefinition& AttributeDefinition::operator[](quint32 index)
{
    return columndefinitionRef(index);
}

std::vector<IlwisTypes> AttributeDefinition::ilwisColumnTypes() const
{
    std::vector<IlwisTypes> types(_columnDefinitionsByIndex.size());
    for(int col = 0; col < _columnDefinitionsByIndex.size(); ++col){
        types[col] = _columnDefinitionsByIndex[col].datadef().domain()->valueType();
    }
    return types;
}

void AttributeDefinition::clearAttributeDefinitions()
{
    _columnDefinitionsByIndex.clear();
    _columnDefinitionsByName.clear();
}

QVariant AttributeDefinition::checkInput(const QVariant& inputVar, quint32 columnIndex)  const{
    QVariant actualval= inputVar;
    const ColumnDefinition& coldef = columndefinitionRef(columnIndex);
	QVariant::Type vtype = inputVar.type();
    if ( !coldef.datadef().domain<>().isValid()){
        ERROR2(ERR_NO_INITIALIZED_2,"Domain",coldef.name());
        return QVariant();
    }
    IlwisTypes domtype = coldef.datadef().domain<>()->ilwisType();
    IlwisTypes valueType = coldef.datadef().domain<>()->valueType();
    IDomain dm = coldef.datadef().domain<>();
    if ( domtype == itITEMDOMAIN){
        if ( inputVar == sUNDEF){
            return QVariant((int)iUNDEF);
        } else if ( vtype == QMetaType::QString){
            actualval = dm->impliedValue(inputVar);

            SPItemRange rng2 = coldef.datadef().range<ItemRange>();

            if ( !hasType(valueType,itINDEXEDITEM)){
                SPItemRange rng1 = dm->range<ItemRange>();
                SPDomainItem item = rng1->item(inputVar.toString());
                if ( item.isNull()){
                    return QVariant((int)iUNDEF);
                }
                if ( !rng2->contains(item->name())){
                    rng2->add(item->clone());
                }
                actualval = item->raw();
            }
		}
		else if (vtype == QMetaType::Double || vtype == QMetaType::Int || vtype == QMetaType::UInt) {
			SPItemRange rng2 = coldef.datadef().range<ItemRange>();

			if (!hasType(valueType, itINDEXEDITEM)) {
				SPItemRange rng1 = dm->range<ItemRange>();
				SPDomainItem item = rng1->item(inputVar.toUInt());
				if (item.isNull()) {
					return QVariant((int)iUNDEF);
				}
				if (!rng2->contains(item->name())) {
					rng2->add(item->clone());
				}
				actualval = item->raw();
			}
		}
    }else if ( domtype == itNUMERICDOMAIN){
        if (vtype == QMetaType::QString)
            actualval =  dm->impliedValue(inputVar);
        if ( hasType(valueType,itDATETIME) && actualval.value<Ilwis::Time>() == tUNDEF)
            return actualval;
        else if (isNumericalUndef(actualval.toDouble()))
            return rUNDEF;
        if (! dm->contains(actualval))
            actualval = rUNDEF;
        else {
            SPNumericRange actualRange = coldef.datadef().range<NumericRange>();
            if ( !actualRange.isNull() ){
                if ( hasType(valueType,itDATETIME))
                    actualRange->add(actualval.value<Ilwis::Time>());
                else {
                    double v = actualval.toDouble();
                    actualRange->add(v);
                }
            }
        }
    } else if ( domtype == itTEXTDOMAIN){
        return dm->impliedValue(inputVar);
    }
    return actualval;
}

quint32 AttributeDefinition::definitionCount() const
{
    return (quint32)_columnDefinitionsByIndex.size();
}

bool AttributeDefinition::isValid() const
{
    return _columnDefinitionsByIndex.size() != 0;
}


//-------------------------------------------------------------------------------------------
void SubFeatureDefinition::setSubDefinition(const IDomain &dom, const std::vector<QString> &items)
{
    _index2subFeature.clear();
    for( auto item : items){
        if ( !dom->contains(item)) {
            continue;
        }
        _index2subFeature.push_back(item);
    }

    _subFeatureDomain = dom;
    _subFeature2Index.clear();
    int count = 0;
    for(auto item : _index2subFeature){
        _subFeature2Index[item] = count;
        ++count;
    }
}

void SubFeatureDefinition::setSubDefinition(const IDomain &dom, const std::vector<double> &items)
{
    _index2subFeature.clear();
    _subFeatureDomain = dom;
    for( auto item : items){
        if ( !dom->contains(item)) {
            continue;
        }

        if ( dom->valueType() == itINTEGER){
            _index2subFeature.push_back(QString::number((qint64)item));
        }else {
            _index2subFeature.push_back(QString::number(item));
        }
    }
    int count = 0;
     _subFeature2Index.clear();
    for(auto item : _index2subFeature){
        _subFeature2Index[item] = count;
        ++count;
    }
}

void SubFeatureDefinition::setSubDefinition(quint32 number)
{
    _index2subFeature.clear();
	_subFeatureDomain.prepare("count");
    for(quint32 i = 0; i < number; ++i){
        QString key = QString::number(i);
        _index2subFeature.push_back(key);
        _subFeature2Index[key] = i;
    }
}

quint32 SubFeatureDefinition::index(const QString &domainItem) const
{
    if ( !_subFeatureDomain.isValid())
        return iUNDEF;

    auto iter = _subFeature2Index.find(domainItem) ;
    if ( iter != _subFeature2Index.end())
        return iter->second;
    return iUNDEF;

}

quint32 SubFeatureDefinition::index(double domainItem) const
{
    if ( !_subFeatureDomain.isValid())
        return iUNDEF;

    double value = _subFeatureDomain->valueType() == itINTEGER ? (qint64)domainItem : domainItem;
    auto iter = _subFeature2Index.find(QString::number(value)) ;
    if ( iter != _subFeature2Index.end())
        return iter->second;
    return iUNDEF;
}

QString SubFeatureDefinition::index(quint32 idx) const
{
    if ( idx < _index2subFeature.size())
        return _index2subFeature[idx];
    return sUNDEF;
}

QString SubFeatureDefinition::insert(const QString& di)
{
    QString domainItem = di;
    if (!_subFeatureDomain.isValid()) {
        kernel()->issues()->log(TR("No valid stack domain; Please set the stack domain before adding bands!"),IssueObject::itWarning);
        return sUNDEF;
    }
    if ( domainItem == sUNDEF) {
        int index = _index2subFeature.size();
        IItemDomain idom = _subFeatureDomain.as<ItemDomain<DomainItem>>();
        if ( idom.isValid()){
            domainItem = idom->item(index)->name();
        }
    }
    if (!_subFeatureDomain->contains(domainItem))
        return sUNDEF;
    auto iter = _subFeature2Index.find(domainItem) ;
    if ( iter == _subFeature2Index.end()) { // insert one item
        _index2subFeature.push_back(domainItem);
        _subFeature2Index[domainItem] = ((quint32)_subFeature2Index.size());
    }
    return domainItem;
}

double SubFeatureDefinition::insert(double domainItem, bool first)
{
    if (!_subFeatureDomain.isValid()) {
        _index2subFeature.clear();
        _subFeatureDomain = IDomain("count");
        _subFeature2Index.clear();
    }
    if ( domainItem == rUNDEF){
        if ( !hasType(_subFeatureDomain->ilwisType(),itNUMERICDOMAIN))
            throw InternalError(TR("Anonymous inserts may only happen with numeric domains"));

        double res = 1.0;
        if ( hasType(_subFeatureDomain->valueType(),itDOUBLE | itFLOAT)){
            res = _subFeatureDomain->range<NumericRange>()->resolution();
            if  ( res == 0)
                res = 0.001;
        }
		domainItem = _index2subFeature.size() != 0 ? _index2subFeature.back().toDouble() + (first ? 0 : res) : 0;
    }
    if (!_subFeatureDomain->contains(domainItem))
        return rUNDEF;
    double value = _subFeatureDomain->valueType() == itINTEGER ? (qint64)domainItem : domainItem;
    auto iter = _subFeature2Index.find(QString::number(value));
    if ( iter == _subFeature2Index.end()) { // insert one item
        if ( _subFeatureDomain->valueType() == itINTEGER){
            _index2subFeature.push_back(QString::number((qint64)domainItem));
            _subFeature2Index[QString::number((qint64)domainItem)] = (quint32)_subFeature2Index.size();
        }else {
            _index2subFeature.push_back(QString::number(domainItem));
            _subFeature2Index[QString::number(domainItem)] = (quint32)_subFeature2Index.size();
        }
    }
    return domainItem;
}

std::vector<QString> SubFeatureDefinition::indexes() const
{
    return _index2subFeature;
}

quint32 SubFeatureDefinition::count() const
{
    return (quint32)_index2subFeature.size();
}

IDomain SubFeatureDefinition::domain() const
{
    return _subFeatureDomain;
}

void SubFeatureDefinition::clearSubFeatureDefinitions()
{
    _index2subFeature.clear();
    _subFeature2Index.clear();
}

bool SubFeatureDefinition::isValid() const
{
    return _subFeatureDomain.isValid();
}

bool SubFeatureDefinition::checkStackDefintion(const SubFeatureDefinition &def)
{
    if ( !def.domain().isValid())
        return false;
    if ( !_subFeatureDomain.isValid())
        return false;
    // domain count is compatible with everything in this context as it is the fallback for unknown stack domains
    if ( def.domain()->code().indexOf("count") != -1)
        return true;
    if ( _subFeatureDomain->code().indexOf("count") != -1)
        return true;
    if ( !def.domain()->isCompatibleWith(_subFeatureDomain.ptr())){
        return false;
    }
    for(const QString& item1 : _index2subFeature ){
        bool found = false;
        for(const QString& item2 : def._index2subFeature ){
            if ( item1 == item2)    {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return true;

}

//------------------------------------------------------------------------------
void FeatureAttributeDefinition::featureAttributeDefinition(FeatureAttributeDefinition *fad)
{
    _subFeatureDefinition.reset(fad);
    _dummy.reset(new FeatureAttributeDefinition());
}

FeatureAttributeDefinition &FeatureAttributeDefinition::featureAttributeDefinitionRef(int level)
{
    if(!_subFeatureDefinition.get())
        return *(_dummy.get());
    if ( level <= 0)
        return *(_subFeatureDefinition.get());
    return _subFeatureDefinition->featureAttributeDefinitionRef(level - 1);
}

const FeatureAttributeDefinition& FeatureAttributeDefinition::featureAttributeDefinition(int level) const
{
    if ( level <= 0)
        return *(_subFeatureDefinition.get());
    return _subFeatureDefinition->featureAttributeDefinition(level - 1);
}

void FeatureAttributeDefinition::copyTo(Ilwis::FeatureAttributeDefinition &def) const
{
    def._dummy.reset(new FeatureAttributeDefinition());
    def._columnDefinitionsByIndex = _columnDefinitionsByIndex;
    def._columnDefinitionsByName = _columnDefinitionsByName;
    def._subFeatureDomain = _subFeatureDomain;
    def._index2subFeature = _index2subFeature;
    def._subFeature2Index = _subFeature2Index;
    if ( _subFeatureDefinition)
        def._subFeatureDefinition.reset(new FeatureAttributeDefinition());
}




