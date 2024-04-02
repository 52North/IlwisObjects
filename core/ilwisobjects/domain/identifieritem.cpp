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
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"


using namespace Ilwis;

IndexedIdentifier::IndexedIdentifier() : _prefix(""),_count(iUNDEF)
{
}

IndexedIdentifier::IndexedIdentifier(const QString &label, quint32 ind, qint32 cnt)
{
    _prefix = label;
    _raw = ind;
    if ( cnt != iUNDEF)
        _count = cnt;
}


QString IndexedIdentifier::name() const
{
    if ( _raw == iUNDEF)
        return _prefix;

    if ( _prefix != "")
        return QString("%1 %2").arg(_prefix).arg(1 + _raw);
    return QString::number(1 + _raw);
}

quint32 IndexedIdentifier::raw() const
{
    return _raw;
}

QString IndexedIdentifier::prefix() const
{
    return _prefix;
}

void IndexedIdentifier::setPrefix(const QString &pf)
{
    _prefix = pf;
}

bool IndexedIdentifier::operator==(const IndexedIdentifier &item) const
{
    return _prefix == item._prefix && _raw == item._raw;
}

DomainItem *IndexedIdentifier::clone() const
{
    return new IndexedIdentifier(_prefix, _raw, _count);
}

bool IndexedIdentifier::isValid() const{
    return true;
}

//QString IndexedIdentifier::itemType() const
//{
//    return "IndexedIdentifier";
//}

ItemRange *IndexedIdentifier::createRange()
{
    return new IndexedIdentifierRange("",0);
}

IlwisTypes IndexedIdentifier::valueTypeS()
{
    return itINDEXEDITEM;
}

IlwisTypes IndexedIdentifier::valueType() const
{
    return valueTypeS();
}

//---------------------------------------------------------------
NamedIdentifier::NamedIdentifier() : _name(sUNDEF)
{
}

NamedIdentifier::NamedIdentifier(const QString &nm, quint32 rawvalue) : DomainItem(rawvalue), _name(nm)
{
}

QString NamedIdentifier::name() const
{
    return _name;
}

void NamedIdentifier::setName(const QString &n)
{
    _name = n;
}

bool NamedIdentifier::operator ==(const NamedIdentifier &item) const
{
    return _name == item.name();
}

IlwisTypes NamedIdentifier::valueType() const
{
    return valueTypeS();
}

NamedIdentifierRange *NamedIdentifier::createRange()
{
    return new NamedIdentifierRange();
}

IlwisTypes NamedIdentifier::valueTypeS()
{
    return itNAMEDITEM;
}

DomainItem *NamedIdentifier::clone() const
{
  NamedIdentifier *nid = new NamedIdentifier(_name);
  nid->_raw = _raw;
  return nid;
}

bool NamedIdentifier::isValid() const {
    return _name != sUNDEF && _name != "";
}


