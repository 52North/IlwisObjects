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
#include "domainitemmodel.h"

using namespace Ilwis;
using namespace Ui;

DomainItemModel::DomainItemModel()
{

}

DomainItemModel::DomainItemModel(Ilwis::SPItemRange range, const QString& name, QObject *parent) :
    QObject(parent), _range(range), _itemname(name)
{
}

QString DomainItemModel::name() const
{
    if ( !_range.isNull())    {
        auto item = _range->item(_itemname);
        if ( !item.isNull())
            return _range->item(_itemname)->name();
    }
    return "";
}

QString DomainItemModel::code() const
{
    if (hasType(_range->valueType(), itTHEMATICITEM | itNUMERICITEM)){
        Ilwis::ThematicRange *thematicrange =  _range->as<Ilwis::ThematicRange>();
        if ( thematicrange){
            Ilwis::SPDomainItem item = thematicrange->item(_itemname);
            if ( !item.isNull()){
                QString codestr = thematicrange->item(_itemname)->as<Ilwis::ThematicItem>()->code();
                if ( codestr != sUNDEF)
                    return codestr;
            }

        }
    }
    return "";
}

QString DomainItemModel::description() const
{
    if ( hasType(_range->valueType(), itTHEMATICITEM | itNUMERICITEM)){
        Ilwis::ThematicRange *thematicrange =  _range->as<Ilwis::ThematicRange>();
        if ( thematicrange){
            Ilwis::SPDomainItem item = thematicrange->item(_itemname);
            if ( !item.isNull())
                return thematicrange->item(_itemname)->as<Ilwis::ThematicItem>()->description();
        }
    }
    return "";
}

QString DomainItemModel::min() const
{
    if (hasType(_range->valueType(), itNUMERICITEM)){
        Ilwis::IntervalRange *irange =  _range->as<Ilwis::IntervalRange>();
        if ( irange){
            Ilwis::SPDomainItem item = irange->item(_itemname);
            if ( !item.isNull()){
                double min = irange->item(_itemname)->as<Ilwis::Interval>()->range().min();
                return QString::number(min);
            }

        }
    }
    return "";
}

QString DomainItemModel::max() const
{
    if (hasType(_range->valueType(), itNUMERICITEM)){
        Ilwis::IntervalRange *irange =  _range->as<Ilwis::IntervalRange>();
        if ( irange){
            Ilwis::SPDomainItem item = irange->item(_itemname);
            if ( !item.isNull()){
                double max = irange->item(_itemname)->as<Ilwis::Interval>()->range().max();
                return QString::number(max);
            }

        }
    }
    return "";
}

void DomainItemModel::name(const QString& n) {
	if (n == _itemname)
		return;
	if (hasType(_range->valueType(), itTHEMATICITEM | itNAMEDITEM | itNUMERICITEM | itTIMEITEM | itPALETTECOLOR)) {
		Ilwis::SPDomainItem item = _range->item(_itemname);
		if (!item.isNull() && n != sUNDEF && n != "") {
			bool duplicate = _range->contains(n);
			if (!duplicate) {
				_range->item(_itemname)->as<Ilwis::NamedIdentifier>()->setName(n);
				_itemname = n;
			}
			else
				kernel()->issues()->log(TR("trying to add duplicate item to domain:") + n);
		}
	}
}
void  DomainItemModel::code(const QString& c){
	if (hasType(_range->valueType(), itTHEMATICITEM | itNUMERICITEM | itTIMEITEM)) {
		Ilwis::SPDomainItem item = _range->item(_itemname);
		if (!item.isNull() && c != sUNDEF && c != "") {
			_range->item(_itemname)->as<Ilwis::ThematicItem>()->code(c);
		}
	}
}
void DomainItemModel::description(const QString& desc) {
	if (hasType(_range->valueType(), itTHEMATICITEM | itNUMERICITEM | itTIMEITEM)) {
		Ilwis::SPDomainItem item = _range->item(_itemname);
		if (!item.isNull() && desc != sUNDEF && desc != "") {
			_range->item(_itemname)->as<Ilwis::ThematicItem>()->description(desc);
		}
	}
}
void DomainItemModel::min(const QString& m) {
	if (hasType(_range->valueType(), itNUMERICITEM)) {
		Ilwis::IntervalRange *irange = _range->as<Ilwis::IntervalRange>();
		if (irange) {
			Ilwis::SPDomainItem item = irange->item(_itemname);
			if (!item.isNull()) {
				double max = irange->item(_itemname)->as<Ilwis::Interval>()->range().max();
				double min = irange->item(_itemname)->as<Ilwis::Interval>()->range().min();
				bool ok;
				double mi = m.toDouble(&ok);
				if (ok && mi < max && mi != min) {
					irange->item(_itemname)->as<Ilwis::Interval>()->rangeRef().min(mi);
				}

			}

		}
	}
}
void DomainItemModel::max(const QString& m) {
	if (hasType(_range->valueType(), itNUMERICITEM)) {
		Ilwis::IntervalRange *irange = _range->as<Ilwis::IntervalRange>();
		if (irange) {
			Ilwis::SPDomainItem item = irange->item(_itemname);
			if (!item.isNull()) {
				double min = irange->item(_itemname)->as<Ilwis::Interval>()->range().min();
				double max = irange->item(_itemname)->as<Ilwis::Interval>()->range().max();
				bool ok;
				double ma = m.toDouble(&ok);
				if (ok && ma > min && ma != max) {
					irange->item(_itemname)->as<Ilwis::Interval>()->rangeRef().max(ma);
				}

			}

		}
	}
}
