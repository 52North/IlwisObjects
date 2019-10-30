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

#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "table.h"
#include "attributemodel.h"
#include "itemdomain.h"
#include "ilwistypes.h"

using namespace Ilwis;
using namespace Ui;

AttributeModel::AttributeModel()
{

}

AttributeModel::~AttributeModel()
{
}

AttributeModel::AttributeModel(const DataDefinition& def, const QString& name, QObject *parent) :
    QObject(parent),
    _datadef(def),
    _name(name)
{
    if ( def.isValid() && def.domain()->ilwisType() == itITEMDOMAIN && def.domain()->valueType() != itINDEXEDITEM){
        if (!def.domain()->range().isNull()){
            QString defset = def.domain()->range()->toString();
            defset = defset.mid(defset.indexOf(":") + 1);
            _attributeValuesDefaultRange = defset.split("|");
        }
        if (!def.range().isNull()){
            QString defset = def.domain()->range()->toString();
            defset = defset.mid(defset.indexOf(":") + 1);
            _attributeValuesActualRange = defset.split("|");
        }
    }
}

AttributeModel::AttributeModel(const DataDefinition &def, const QString &name, QObject *parent, const IIlwisObject &owner) :
    AttributeModel(def, name, parent)
{
    _owner = owner;
}

QString AttributeModel::range() const {
	QString rng;
	if (_datadef.isValid()) {
		if (_datadef.domain()->ilwisType() == itNUMERICDOMAIN) {
			rng = actualRangeDefintion();
			if (rng == "")
				rng = defaultRangeDefinition();
			rng = rng.remove("numericrange:");
			rng.replace("|", ", ");
		}
		else if (_datadef.domain()->ilwisType() == itITEMDOMAIN) {
			IItemDomain itemdom = _datadef.domain().as< ItemDomain<DomainItem>>();
			IlwisTypes vt = itemdom->valueType();
			rng = QString::number(itemdom->count()) + " items";
		}

	}
	return rng;
}

QString AttributeModel::defaultRangeDefinition() const
{
    if ( _datadef.isValid()){
         if ( !_datadef.domain()->range().isNull())
            return _datadef.domain()->range()->toString();
    }
    return "";
}

QString AttributeModel::actualRangeDefintion(bool calc) const
{
    QString rangeString;
    if ( _datadef.isValid()){
        if ( calc || _datadef.range().isNull()){
            if ( hasType( _owner->ilwisType() , itFEATURE)){
                Ilwis::IFeatureCoverage features = _owner.as<Ilwis::FeatureCoverage>();
                if ( !features.isValid())
                    return "";
                features->loadData();
                std::vector<QVariant> data = features->attributeTable()->column(_name);
                if ( data.size() != 0){
                    std::vector<double> values(data.size());
                    int  i=0;
                    for(auto v : data)
                        values[i++] = v.toDouble();
                    Ilwis::NumericStatistics stats; // realy like to do this with a template specialization of the proper calculate function, but the syntax was unclear to me
                    stats.calculate(values.begin(), values.end());
                    Ilwis::NumericRange *rng = new Ilwis::NumericRange(stats.prop(Ilwis::NumericStatistics::pMIN),stats.prop(Ilwis::NumericStatistics::pMAX));
                    features->attributeDefinitionsRef().columndefinitionRef(_name).datadef().range(rng);
                    const_cast<AttributeModel *>(this)->_datadef = features->attributeDefinitions().columndefinition(_name).datadef();
                    const_cast<AttributeModel *>(this)->_name = _name;
                }
            }
        }
        rangeString = _datadef.range()->toString();

    }
    return rangeString;
}

QString AttributeModel::attributename() const
{
   return _name;
}

QString AttributeModel::attributeDomain() const
{
  if ( _datadef.isValid()){
      if ( _datadef.domain()->ilwisType() == itTEXTDOMAIN)
      	return "text"; // all text domains are unique with a anonymous name; you don't want that in the UI
      return _datadef.domain()->name();

  }
  return "";
}

QString AttributeModel::attributeValueType() const
{
    if ( _datadef.isValid()){

        QString typeName =  Ilwis::TypeHelper::type2HumanReadable( _datadef.domain()->valueType());

        return typeName == sUNDEF ? "" : typeName;
    }
    return "";
}

QString AttributeModel::attributeInternalDomainType() const {
	if (!_datadef.isValid())
		return "";
	quint64 tp = _datadef.domain()->ilwisType();
	return Ilwis::TypeHelper::type2name(tp);
}
QString AttributeModel::attributeDomainType() const
{
    if ( !_datadef.isValid())
        return "";
    quint64 tp = _datadef.domain()->ilwisType();
    return Ilwis::TypeHelper::type2HumanReadable(tp);
}

QStringList AttributeModel::attributeValues(bool defaultRange) const
{
    if ( defaultRange)
        return _attributeValuesDefaultRange;
    return _attributeValuesActualRange;
}

QString AttributeModel::rangeDefinition(bool defaultRange, bool calc, const QString&)  const
{
    if ( defaultRange)
        return defaultRangeDefinition();
    return actualRangeDefintion(calc);
}

const DataDefinition &AttributeModel::datadefinition() const
{
    return _datadef;
}

void AttributeModel::setObject(const Ilwis::IIlwisObject &obj)
{
    _owner = obj;
}

QString AttributeModel::representationId() const {
	if (_datadef.isValid() && _datadef.representation().isValid())
		return QString::number(_datadef.representation()->id());
	return "";
}

QString AttributeModel::icon()
{
    if ( !_datadef.isValid())
        return "";
    IlwisTypes tp = _datadef.domain()->valueType();
    if ( hasType(tp,itNUMBER)){
        return "numericdomain.png";
    }
    if ( hasType(itDOMAINITEM, tp)){
        return "itemdomain.png";
    }
    if ( hasType(itSTRING, tp)){
        return "textdomain.png";
    }
    return "";

}
