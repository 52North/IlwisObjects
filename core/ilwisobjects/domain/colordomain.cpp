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

#include <QColor>
#include <QSharedPointer>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "datadefinition.h"
#include "itemrange.h"
#include "domainitem.h"
#include "coloritem.h"
#include "colorrange.h"

#include "colordomain.h"

using namespace Ilwis;



ColorDomain::ColorDomain()
{
}

ColorDomain::ColorDomain(const Resource &resource) : Domain(resource)
{

}

IlwisTypes ColorDomain::valueType() const
{
    if ( !_range.isNull())
        return _range->valueType();
    return itUNKNOWN;
}

QVariant ColorDomain::impliedValue(const QVariant &value) const
{
    if ( !_range.isNull())
        return _range->impliedValue(value);
    return QVariant();
}

Domain::Containement ColorDomain::contains(const QVariant &value) const
{
    if ( !_range.isNull()) {
        if ( !_range.isNull() && _range->contains(value))
            return Domain::cSELF;
        if (parent().isValid())
            if (parent()->contains(value) == Domain::cSELF)
                return Domain::cPARENT;
    }
    return Domain::cNONE;
}

bool ColorDomain::isCompatibleWith(const IlwisObject* obj, bool strict) const
{
    if ( !obj || !obj->isValid())
        return false;
    if ( obj->ilwisType() != itCOLORDOMAIN)
        return false;
    return true;
}

IlwisTypes ColorDomain::ilwisType() const
{
    return itCOLORDOMAIN;
}

void ColorDomain::range(Range *colorrange)
{
    if ( isReadOnly())
        return;
    changed(true);
    if ( hasType(colorrange->valueType(), itCOLOR) == false)
        return;
    if ( parent().isValid()) {
        if ( colorrange->valueType() == itPALETTECOLOR){
            if( cNONE == parent()->range<ColorPalette>()->contains(colorrange))
                return;
        } else if ( colorrange->valueType() == itCONTINUOUSCOLOR){
            if( cNONE == parent()->range<ContinuousColorRange>()->contains(colorrange))
                return;
        }
    }
    _range = QSharedPointer<Range>(colorrange);
}

bool ColorDomain::isOrdered() const
{
    return false;
}

QSharedPointer<Range> ColorDomain::getRange() const
{
    return _range;
}

void ColorDomain::setParent(const IDomain& dm){

}

IlwisObject *ColorDomain::clone()
{
    auto *colordom = new ColorDomain();
    copyTo(colordom);
    return colordom;
}

void ColorDomain::copyTo(IlwisObject *obj){
    Domain::copyTo(obj);
    ColorDomain *colordom = static_cast<ColorDomain *>(obj);
    colordom->_range.reset( _range->clone());
}


