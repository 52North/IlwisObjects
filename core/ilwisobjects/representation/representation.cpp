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
#include "domain.h"
#include "itemdomain.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "colorlookup.h"
#include "continuouscolorlookup.h"
#include "palettecolorlookup.h"
#include "representation.h"

using namespace Ilwis;

Representation::Representation()
{
}

Representation::Representation(const Resource &resource) : IlwisObject(resource)
{

}

//many rpr's are created as a system rpr. the moment elements are changed they morph into
//  a user defined rpr. this method does that transofmration
IlwisData<Representation> Representation::copyWith(const IDomain &dom) const
{
    IRepresentation rpr;
    QString rprurl; // we don't want to reuse an 'old' copy but a new one so we search until we find an available new name
    if (!dom->isAnonymous()) { // loop to find a nice non-existing rprurl
        QString name = dom->name();
        if (name.indexOf(".") != -1)
            name = name.left(name.indexOf("."));

        QString rprurl_base = INTERNAL_CATALOG + "/" + name;
        rprurl = rprurl_base;
        quint64 id = i64UNDEF;
        int count = 0;
        id = mastercatalog()->name2id(rprurl, itREPRESENTATION);
        while (id != i64UNDEF && mastercatalog()->isRegistered(id)) {
            rprurl = rprurl_base + "_" + QString::number(++count);
            id = mastercatalog()->name2id(rprurl, itREPRESENTATION);
        }
    } else { // just assign the next available anonymous url
        rprurl = INTERNAL_CATALOG + "/" + Identity::newAnonymousName();
    }
    rpr.prepare(rprurl); // prepare a new empty representation with this new url
    if (rpr.isValid()) {
        // assign the domain that should go along
        rpr->domain(dom);
        // clone the colors and the shapes
        if (colors())
            rpr->colors(colors()->clone());
        if (shapes().get() != 0) {
            rpr->shapes(shapes()->clone());
        }
    }
    return rpr;

}

const UPColorLookUp &Representation::colors() const
{
        return _colors;
}

UPColorLookUp &Representation::colors() 
{
	return _colors;
}

void Representation::colors(ColorLookUp *lookup)
{
    if (isReadOnly())
        return;
    changed(true);

    _colors.reset(lookup);

}

void Representation::shapes(ShapeLookUp *lookup)
{
    if (isReadOnly())
        return;
    changed(true);

    _shapes.reset(lookup);
}

const UPShapeLookUp &Representation::shapes() const
{
    return _shapes;
}

IDomain Representation::domain() const
{
    return _domain;
}

void Representation::domain(const IDomain &domain)
{
    if (isReadOnly())
        return;
	if (domain == _domain)
		return;
    changed(true);
    if (!_colors || (!isCompatible(domain) && _domain.isValid())){
        _colors.reset(ColorLookUp::create(domain));
	}
	else if (_colors) {
		_colors->reset(domain);
	}

    _domain = domain;
}

bool Representation::isCompatible(const IDomain &otherDomain)
{
    if ( domain().isValid() && otherDomain.isValid()){
        if ( otherDomain == domain())
            return true;

        if ( hasType(domain()->valueType(), itNUMBER) && hasType(otherDomain->valueType(), itNUMBER)){
            return true;
        }
        if ( domain()->ilwisType() == itITEMDOMAIN && otherDomain->ilwisType() == itITEMDOMAIN){
            IItemDomain dom1 = domain().as<ItemDomain<DomainItem>>();
            IItemDomain dom2 = otherDomain.as<ItemDomain<DomainItem>>();
            if ( dom1->count() == dom2->count()){
                return true;
            }


        }
    }

    return false;
}

IlwisTypes Representation::ilwisType() const
{
    return itREPRESENTATION;
}

IlwisObject *Representation::clone()
{
	Representation * rpr = new Representation();
	copyTo(rpr);
	return rpr;
}

void Representation::copyTo(IlwisObject *obj) {
	IlwisObject::copyTo(obj);
	Representation *rpr = static_cast<Representation *>(obj);
	rpr->_colors.reset(_colors->clone());
	rpr->_shapes.reset(_shapes->clone());
	rpr->_domain = _domain;

}

IlwisData<Representation> Representation::defaultRepresentation(const IDomain &dom)
{

	QString code("code=rpr:monochromeblack"); // default
	if (hasType(dom->ilwisType(), itNUMERICDOMAIN)) {
        NumericRange *numrange = dom->range()->as<NumericRange>();
		if (numrange->resolution() == 1)
			code = "code=rpr:greyscale";
		else
			code = "code=rpr:pseudo2";
	}else if ( hasType(dom->ilwisType(), itITEMDOMAIN )){
        code = "code=rpr:primarycolors";
    } else if ( hasType(dom->ilwisType(), itTEXTDOMAIN) ){
        code = "code=rpr:primarycolors";
    }
	auto temp = IRepresentation(code);
	Resource res = temp->resource();
	quint64 newId = Identity::newAnonymousId();
	QString path = INTERNAL_CATALOG + "/" + ANONYMOUS_PREFIX + QString::number(newId);
	res.setId(newId);
	res.setUrl(path, false, false);
	res.setUrl(path, true, false);
    IRepresentation newRpr =  IRepresentation(res);
	newRpr->domain(dom);
	return newRpr;
}

bool Representation::isValid() const {
    return _colors && _domain.isValid();
}
