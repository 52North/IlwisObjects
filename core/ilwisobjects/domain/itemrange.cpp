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
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"

using namespace Ilwis;

QHash<QString, CreateItemFunc> ItemRange::_createItem;

bool ItemRange::contains(SPRange rng, bool ) const
{
    SPItemRange itemrng = rng.staticCast<ItemRange>();
    for(int  i=0; i < itemrng->count(); ++i) {
        if(!contains(itemrng->itemByOrder(i)->name()))
            return false;
    }
    return true;
}

bool ItemRange::contains(const ItemRange &itemrng, bool ) const
{
    for(int  i=0; i < itemrng.count(); ++i) {
        if(!contains(itemrng.itemByOrder(i)->name()))
            return false;
    }
    return true;
}

QVariant ItemRange::ensure(const QVariant &v, bool ) const
{
    if ( contains(v))
        return v;
    return QVariant();
}

DomainItem *ItemRange::create(const QString& type){
    auto iter = _createItem.find(type);
    if ( iter != _createItem.end())
        return iter.value()(type);
    return 0;
}

void ItemRange::addCreateItem(const QString& type, CreateItemFunc func){
    _createItem[type] = func;
}

ItemRange *ItemRange::merge(const QSharedPointer<ItemRange> &nr1, const QSharedPointer<ItemRange> &nr2)
{
    //TODO:
    return 0;
}

bool ItemRange::isOrdered() const
{
    return false;
}

void ItemRange::addRange(const ItemRange &range)
{
    for(quint32 i=0; i < range.count(); ++i) {
        if (!contains(range.item(i)->name()))
            add(range.item(i)->clone());
    }

}

SPDomainItem ItemRange::valueAt(quint32& index, const Range *rng){
    if ( rng && hasType(rng->valueType(), itDOMAINITEM)){
        const ItemRange *idrange = static_cast<const ItemRange *>(rng);
        if ( index < idrange->count()){
            return idrange->item(index);
        }
    }
    index = iUNDEF;
    return SPDomainItem();
}

QString ItemRange::valueAsString(quint32 &index, const Range *rng)
{
    SPDomainItem item = valueAt(index, rng);
    return item->name();
}

quint32 ItemRange::overlapCount(const QString& item) {
	return 0;
}

bool ItemRange::hasOverlaps() const {
	return false;
}


