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
#include <typeinfo>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domainitem.h"
#include "numericrange.h"
#include "domainitem.h"
#include "domain.h"
#include "itemdomain.h"
#include "interval.h"
#include "itemrange.h"
#include "intervalrange.h"

using namespace Ilwis;

IntervalRange::IntervalRange()
{
}

QVariant IntervalRange::impliedValue(const QVariant& v) const
{
    bool ok;
    quint32 index = v.toUInt(&ok);
    if (!ok){
		QString typName = v.typeName();
		if (typName == "QString") {
			SPNamedIdentifier it = item(v.toString()).staticCast<NamedIdentifier>();
			if (it)
				return it->raw();
			if (v.toString() == "" || v.toString() == sUNDEF) { // undefines are acceptable values
				return QVariant();
			}
		}
		ERROR2(ERR_COULD_NOT_CONVERT_2, v.toString(), "raw value");
		return QVariant();
    }
    if ( index < _items.size())
        return _items[index]->name();
    return sUNDEF;

}

quint32 IntervalRange::count() const
{
    return (quint32)_items.size();
}

SPDomainItem IntervalRange::item(quint32 index) const
{
       if ( index < _items.size())
           return _items[index];
       return SPDomainItem();
}

quint32 IntervalRange::overlapCount(const QString& item) {
	int count = 0;
	for (quint32 ind = 0; ind < _items.size(); ++ind) {
		const NumericRange& rng = _items[ind]->range();
		for (quint32 ind2 = 0; ind2 < _items.size(); ++ind2) {
			if (ind != ind2) {
				if (_items[ind2]->range().overlaps(&rng))
					++count;

			}
		}
	}
	return count;
}

bool IntervalRange::hasOverlaps(bool strict) const {
	if (_items.size() == 0)
		return false;

	for (quint32 ind = 0; ind < _items.size(); ++ind) {
		const NumericRange& rng = _items[ind]->range();
		for (quint32 ind2 = 0; ind2 < _items.size(); ++ind2) {
			if (ind != ind2) {
				if (_items[ind2]->range().overlaps(&rng, strict))
					return true;
			}
		}
	}
	return false;
}

SPDomainItem IntervalRange::item(const QString &item, int itemIndex) const
{
    QStringList parts = item.split("|");
    if ( parts.size() == 0 || parts.size() > 3){
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), item);
        return SPDomainItem();
    }
    bool ok;
    auto v1 = index(parts[0].toDouble(&ok));
    if ( v1.size() == 0  && ok) {
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), item);
        return SPDomainItem();
    }
    if (parts.size() > 1 && ok) {
        auto v2 = index(parts[1].toDouble(&ok));
        if ( !ok ) {
            ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), item);
            return SPDomainItem();
        }
		else if (v1.size() == 0 && v2.size() != 0) {
			v1 = v2;
		}
		else if (v1.size() != 0 && v2.size() != 0) {
			for (int v : v2) {
				auto iter = std::find(v1.begin(), v1.end(), v);
				if (iter != v1.end())
					v1.push_back(v);
			}
		}

    }
    if (!ok){
        for(auto val : _items) {
            if ( val->name().trimmed() == item.trimmed()){
                return val;
            }
        }
        return SPDomainItem();
    }
	if ( itemIndex == iUNDEF)
		return _items[v1[0]];
	if (itemIndex < v1.size()) {
		return _items[v1[itemIndex]];
	}
	return SPDomainItem();
}

SPDomainItem IntervalRange::itemByOrder(quint32 index) const
{
    return item(index);
}

std::vector<int> IntervalRange::index(double v, int itemIndex) const
{
	std::vector<int> result;
	if (_items.size() != 0) {
		std::vector<int> result;
		for (int ind = 0; ind < _items.size(); ++ind) {
			if (_items[ind]->range().contains(v)) {
				if (itemIndex == iUNDEF)
					return { ind };
				result.push_back(ind);
			}
		}
	}
    return result;
}

bool IntervalRange::validNumber(QString value) const{
    bool oknumber;
    double v = value.toDouble(&oknumber);
    if (!oknumber) {
        return false;
    }

    return index(v).size() != 0;

}

bool IntervalRange::contains(const QVariant &name, bool ) const
{
    QStringList items = name.toString().split(";");
    for(const QString& item : items) {
        QStringList parts = item.split("|");
        if ( parts.size() == 0 || parts.size() > 3){
            return ERROR2(ERR_ILLEGAL_VALUE_2, TR("numeric range"), name.toString());
        }
        bool ok = validNumber(parts[0]);
        if (parts.size() > 1)
            ok &= validNumber(parts[1]);
        if (!ok && parts.size() == 1){ // maybe we checked for the label
            for(const auto& item : _items)    {
                if (item->name().trimmed() == parts[0].trimmed()){
                    return true;
                }
            }
        }
        return ok;
    }
    return true;
}

bool IntervalRange::isValid() const
{
	if (_items.size() == 0)
		return false;
	/*std::map<double, SPInterval> ordered;
	for (auto interval : _items) {
		ordered[interval->range().min()] = interval;
	}
	if (ordered.size() < _items.size()) {
		kernel()->issues()->log(TR("overlapping interval defintion"), IssueObject::itWarning);
		return false;
	}
	double prevmax=rUNDEF;
	for (auto ordItem : ordered) {
		double omin = ordItem.second->range().min();
		double omax = ordItem.second->range().max();
		if (prevmax == rUNDEF) {
			prevmax = omax;
			if (omin > prevmax) {
				kernel()->issues()->log(TR("wrong order of min/max in interval"), IssueObject::itWarning);
				return false;
			}
		}
		else {
			if (prevmax >omin ) {
				kernel()->issues()->log(TR("wrong order of min/max in interval"), IssueObject::itWarning);
				return false;
			}
			if (omin > omax) {
				kernel()->issues()->log(TR("wrong order of min/max in interval"), IssueObject::itWarning);
				return false;
			}
			prevmax = ordItem.second->range().max();
		}
	}*/
	return true;

}

void IntervalRange::add(DomainItem *item)
{
    if (!item)
        return;

    if ( item->valueType() != itNUMERICITEM)
        return;

    if (!item->isValid())
        return;
    bool inserted = false;
    SPInterval nitem(static_cast<Interval *>(item));
    for(auto iter = _items.rbegin(); iter != _items.rend(); ++iter) {
        if ( nitem->range().max() < (*iter)->range().min()) {
            if ( nitem->raw() == iUNDEF)
                nitem->_raw = (quint32)_items.size();
            _items.insert(iter.base(),1, nitem );
            inserted = true;
            break;
        }
    }
    if ( !inserted) { // no overlapping items allowed; so the only case that is legal here is the first
        nitem->_raw = _items.size();
        _items.push_back(nitem);
    }
}

void IntervalRange::add(SPDomainItem item)
{
    if ( item.isNull() || !item->isValid() || item->valueType() != itNUMERICITEM)
        return;

    SPInterval nitem = item.staticCast<Interval>();
    for(auto iter = _items.rbegin(); iter != _items.rend(); ++iter) {
        if ( nitem->range() > (*iter)->range()) {
            if ( nitem->raw() == iUNDEF)
                nitem->_raw = (quint32)_items.size();
            _items.insert(iter.base(),1, nitem );
            return;
        }
    }
    if ( _items.size() == 0) { // no overlapping items allowed; so the only case that is legal here is the first
        nitem->_raw = 0;
        _items.push_back(nitem);
    }
}

void IntervalRange::add(const QVariant &iditem)
{
    if ( contains(iditem))
        return;
    QStringList items = iditem.toString().split(";");
    for(const QString& item : items) {
        QStringList parts = item.split(" ");
        if ( parts.size() == 0 || parts.size() > 3){
            return ;
        }
        add(new Interval(parts[0], NumericRange(parts[1].toDouble(), parts[2].toDouble())));
    }

}

NumericRange IntervalRange::totalRange() const {
	NumericRange result;
	double rmin = 1e308, rmax = -1e308, res=0;
	for (auto item : _items) {
		rmin = std::min(rmin, item->rangeRef().min());
		rmax = std::max(rmax, item->rangeRef().max());
		res = item->rangeRef().resolution();
	}
	if (rmin != rUNDEF) {
		result = NumericRange(rmin, rmax, res);
	}
	return result;
}

bool IntervalRange::alignWithParent(const IDomain &dom)
{
    IIntervalDomain numdom = dom.as<IntervalDomain>();
    if ( !numdom.isValid())
        return false;
    ItemIterator<Interval> iterParent(numdom);
    std::map<QString, DomainItem *> parentItems;
    for(DomainItem *item : iterParent) {
        parentItems[item->name()] = item;
    }

    for(int i=0; i < _items.size(); ++i) {
        // TODO:: a bit more flexible in the future(maybe?) with overlapping ranges; absent step sizes
        auto iter = parentItems.find(_items[i]->name());
        if ( iter == parentItems.end()){
            return ERROR2(ERR_ILLEGAL_VALUE_2, TR("item in child domain"),_items[i]->name());
        }
        _items[i] = SPInterval(static_cast<Interval *>((*iter).second->clone()));
    }
    return true;

}

void IntervalRange::remove(const QString &nm)
{
     for(auto iter = _items.begin(); iter != _items.end(); ++iter) {
        if ( (*iter)->name() == nm) {
            _items.erase(iter);
            break;
        }
     }
}

QString IntervalRange::toString() const
{   if ( _items.size() == 0)
        return sUNDEF;
    QString names;
    for(const SPInterval& it: _items) {
        if ( names != "")
            names += "|";
        names += it->name();
    }
    return "intervalrange:"+ names;
}

void IntervalRange::store(QDataStream &stream)
{
    quint32 size = (quint32)_items.size();
    stream << size;
    for(const auto& item : _items){
        stream << item->raw() << item->name() << item->description();
        item->range().store(stream);
    }
}

void IntervalRange::load(QDataStream &stream)
{
    quint32 size;
    stream >> size;
    for(int i =0; i < size; ++i)    {
        QString label, desc;
        quint32 raw;
        stream >> raw >> label >> desc;
        NumericRange rng;
        rng.load(stream);
        Interval *interval = new Interval(label, rng);
        interval->description(desc);
        add(interval);
    }
}

void IntervalRange::clear()
{
    _items.clear();
}

ItemRange *IntervalRange::clone() const
{
    ItemRange *items = new IntervalRange();
    for(const SPInterval& it: _items) {
        items->add(it->clone());
    }
    items->interpolation(interpolation());

    return items;

}

IntervalRange &IntervalRange::operator <<(const QString &itemdef)
{
    QStringList parts = itemdef.split("|");
    if( parts.size() > 2) {
        bool ok;
        double step = 0;
        QString name = parts[0];
        double mn = parts[1].toDouble(&ok);
        if(ok) {
            double mx = parts[2].toDouble(&ok);
            if ( ok) {
                if ( parts.size() == 4) {
                    step = parts[3].toDouble(&ok);

                }
                if (ok) {
                    add(new Interval(name, {mn,mx, step}));
                    return *this;
                }
            }
        }
        ERROR1(ERR_NO_INITIALIZED_1, "numeric item range");
    } if ( parts.size() == 1) {
        if ( count() == 0) {
            ERROR1(ERR_NO_INITIALIZED_1, "numeric item range");
            return *this;
        }
        bool ok;
        double vmin = _items[_items.size() - 1]->range().max();
        double vmax = itemdef.toDouble(&ok) ;
        if (!ok) {
            ERROR1(ERR_NO_INITIALIZED_1, "numeric item range");
        }
        double step = _items[0]->range().resolution();
        if ( step == 0)
            vmin += EPS8;
        QString label = QString("label_%1").arg(_items.size());
        add(new Interval(label, {vmin+step,vmax, step}));
    }
    return *this;
}

qint32 IntervalRange::gotoIndex(qint32 index, qint32 step) const
{
    if ( index + step >= _items.size())
        return (qint32)_items.size();
    if ( index + step < 0)
        return 0;
    return index + step;
}

IlwisTypes IntervalRange::valueType() const
{
    return itNUMERICITEM;
}

void IntervalRange::addRange(const ItemRange &range)
{
    ItemRange::addRange(range);
    _interpolation = static_cast<const IntervalRange&>(range).interpolation();
}

SPDomainItem IntervalRange::valueAt(quint32 index, const Range *rng){
    if ( rng && hasType(rng->valueType(), itNUMERICITEM)){
        const IntervalRange *idrange = static_cast<const IntervalRange *>(rng);
        if ( index < idrange->count()){
            return idrange->item(index);
        }
    }
    return SPDomainItem();
}

QString IntervalRange::valueAsString(quint32 index, const Range *rng)
{
    SPDomainItem item = valueAt(index, rng);
    return item->name();
}



