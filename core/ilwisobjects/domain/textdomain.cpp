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

#include <QUrl>
#include "module.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "mastercatalog.h"
#include "textdomain.h"

using namespace Ilwis;
TextDomain::TextDomain()
{
}

TextDomain::TextDomain(const Resource &resource) : Domain(resource)
{
}

IlwisTypes TextDomain::valueType() const
{
    return itSTRING;
}

QVariant TextDomain::impliedValue(const QVariant &v) const
{
    return v.toString();
}

Domain::Containement TextDomain::contains(const QVariant &) const
{
    return Domain::cSELF;
}

bool TextDomain::isCompatibleWith(const IlwisObject *obj, bool strict) const {
    if ( !obj->isValid())
        return false;
    if(obj->ilwisType() != itTEXTDOMAIN)
        return false;
    return true;
}

bool TextDomain::isOrdered() const
{
    return false;
}

IlwisTypes TextDomain::ilwisType() const
{
    return itTEXTDOMAIN;
}

QSharedPointer<Range> TextDomain::getRange() const
{
    return QSharedPointer<Range>();
}
