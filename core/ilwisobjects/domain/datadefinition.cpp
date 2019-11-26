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

#include <QString>
#include "module.h"
#include "kernel.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domainitem.h"
#include "domain.h"
#include "numericdomain.h"
#include "itemdomain.h"
#include "range.h"
#include "numericrange.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "itemrange.h"
#include "representation.h"
#include "containerstatistics.h" 
#include "datadefinition.h"

using namespace Ilwis;

DataDefinition::DataDefinition() : _range(0)
{
}


DataDefinition::DataDefinition(const DataDefinition& def)
{
    operator=(def);
}

DataDefinition::DataDefinition(const IDomain &dm, Range *rng)
{
    domain(dm);
    if ( rng)
        _range.reset(rng);
	_representation = Representation::defaultRepresentation(domain());
}

DataDefinition::~DataDefinition()
{
    _domain.set(0);
}

DataDefinition &DataDefinition::operator =(const DataDefinition &def)
{
    domain(def.domain<>());
    if ( !def.range().isNull())
        _range.reset(def.range()->clone());
    else
        _range.reset(0);
	_representation = def._representation;
	_statistics = def._statistics;

    return *this;
}

void DataDefinition::range(Range* vr)
{
    _range.reset(vr);
}

void DataDefinition::domain(const IDomain &dom)
{
    _domain = dom;
    if ( !_domain.isValid())
        return;

    SPRange r = _domain->range<>();
    if (r) {
        _range.reset(r->clone());
        _range->clear(); // the range(type) is the same but it may not contain values yet 
    }
	_representation = Representation::defaultRepresentation(domain());

}


bool DataDefinition::isValid() const
{
    return _domain.isValid();
}

bool DataDefinition::isCompatibleWith(const DataDefinition &def) const
{
    if (!isValid())
        return false;

    return _domain->isCompatibleWith(def.domain<>().ptr());

}


DataDefinition DataDefinition::merge(const DataDefinition &def1, const DataDefinition &def2)
{
    IDomain dm;
    DataDefinition def;
    if ( !def1.isValid() && def2.isValid())
        return def2;
    if ( !def2.isValid() && def1.isValid())
        return def1;

    if ( def1.domain()->ilwisType() == itNUMERICDOMAIN && def1.domain()->ilwisType() == itNUMERICDOMAIN) {
        if ( def1.domain()->code() == "boolean" && def2.domain()->code() == "boolean") {
            def = DataDefinition(def1);
        }
        SPNumericRange nr1 = def1.range().dynamicCast<NumericRange>();
        SPNumericRange nr2 = def1.range().dynamicCast<NumericRange>();
        NumericRange *nrNew = NumericRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def2.domain()->name()) {
            def = DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("value");
            def = DataDefinition(dm, nrNew);
        }
        return def;

    }
    if ( def1.domain()->ilwisType() == itITEMDOMAIN && def1.domain()->ilwisType() == itITEMDOMAIN) {
        SPItemRange nr1 = def1.range().dynamicCast<ItemRange>();
        SPItemRange nr2 = def1.range().dynamicCast<ItemRange>();
        ItemRange *nrNew = ItemRange::merge(nr1, nr2);
        if ( def1.domain()->name() == def2.domain()->name()) {
            return DataDefinition(def1.domain(), nrNew);
        } else {
            dm.prepare("blavla");
            return DataDefinition(dm, nrNew);
        }
    }
    return DataDefinition();
}

void DataDefinition::representation(const IRepresentation& rpr) {
	if (rpr->domain()->isCompatibleWith(_domain.ptr())) {
		_representation = rpr;
	}
}

IRepresentation DataDefinition::representation() const {
	return _representation;
}

NumericStatistics& DataDefinition::statisticsRef() {
	return _statistics;
}

NumericStatistics DataDefinition::statistics() const {
	return _statistics;
}

//-----------------------------------------------------------
bool Ilwis::operator==(const DataDefinition &def1, const DataDefinition &def2)
{
    return def1.domain() == def2.domain();
}

bool Ilwis::operator!=(const DataDefinition &def1, const DataDefinition &def2)
{
    return !operator==(def1, def2);
}
