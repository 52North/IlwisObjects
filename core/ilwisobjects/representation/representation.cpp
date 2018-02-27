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
    QString name = dom->name();
    if (name.indexOf(".") != -1)
        name = name.left(name.indexOf("."));

    QString rprurl = INTERNAL_CATALOG + "/" + name;
    quint64 id = i64UNDEF;
    int count = 0;
    // we don't want to reuse an 'old' copy but a new one so we search until we find an available new name
    while((id = mastercatalog()->name2id(rprurl, itREPRESENTATION))!= i64UNDEF){
        rpr.prepare(id);
        if (rpr.isValid()){
            rprurl = rprurl + "_" + QString::number(++count);
        }else
            break;
    }

    if ( rpr.isValid() && rpr->isValid())
        return rpr;

    if ( !rpr.isValid())
        rpr.prepare(rprurl, itREPRESENTATION);
    rpr->domain(dom);
    if ( colors())
        rpr->colors(colors()->clone());
    if ( shapes().get() == 0){
        rpr->shapes(shapes()->clone());
    }
    return rpr;

}

const UPColorLookUp &Representation::colors() const
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
    changed(true);
    if (!_colors || (!isCompatible(domain) && _domain.isValid())){
        _colors.reset(ColorLookUp::create(domain));
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

IlwisData<Representation> Representation::defaultRepresentation(const IDomain &dom)
{
    QString code("code=rpr:monochromeblack"); // default
    if ( hasType(dom->ilwisType(), itNUMERICDOMAIN)){
        NumericRange *numrange = dom->range()->as<NumericRange>();
        if ( numrange->min() >= 0 && numrange->max() < 256 && numrange->resolution() == 1)
            code = "code=rpr:greyscale";
        else
            code = "code=rpr:pseudo";
    }else if ( hasType(dom->ilwisType(), itITEMDOMAIN )){
        code = "code=rpr:primarycolors";
    } else if ( hasType(dom->ilwisType(), itTEXTDOMAIN) ){
        code = "code=rpr:primarycolors";
    }

    return IRepresentation(code);
}

bool Representation::isValid() const {
    return _colors && _domain.isValid();
}
