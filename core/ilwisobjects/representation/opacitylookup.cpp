#include <sstream>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "itemrange.h"
#include "domainitem.h"
#include "colorrange.h"
#include "numericrange.h"
#include "opacitylookup.h"

using namespace Ilwis;

std::vector<double> OpacityLookup::values2opacities(const NumericRange &actualRange, const NumericRange &stretchRange, int paletteSize) const
{
    std::vector<double> opacities(paletteSize);
    opacities[0] = 0;
    for (int i = 1; i < paletteSize; ++i) {
        double value = actualRange.min() + actualRange.distance() * ((double)(i - 1) / (paletteSize - 1));
        opacities[i] = value2opacity(value, actualRange, stretchRange);
    }
    return opacities;
}

OpacityLookup *OpacityLookup::create(IlwisTypes domainType, const SPRange& actualRange)
{
    if (hasType(domainType, itITEMDOMAIN | itTEXTDOMAIN)) {
        const ItemRange *irange = actualRange->as<ItemRange>();
        if ( irange)
            return new ItemOpacityLookup(irange);
      
    }
    else if (hasType(domainType, itNUMERICDOMAIN)) {
        const NumericRange *nrange = actualRange->as<NumericRange>();
        if ( nrange)
            return new ContinuousOpacityLookup(nrange);
    }
    return 0;
}

//---------------------------------------------
ContinuousOpacityLookup::ContinuousOpacityLookup(const NumericRange *nrange)
{
}

void ContinuousOpacityLookup::setOpacity(double value, double opacity) {

}

OpacityLookup *ContinuousOpacityLookup::clone() const {
    ContinuousOpacityLookup *newlookup = new ContinuousOpacityLookup(0);
    newlookup->_opacityranges = _opacityranges;
    newlookup->_groups = _groups;
    newlookup->_linear = _linear;
    newlookup->_numericRange = _numericRange;
    newlookup->_step = _step;

    return newlookup;
}

double ContinuousOpacityLookup::value2opacity(double value, const NumericRange& actualRange, const NumericRange& stretchRange ) const {
    if (value == rUNDEF)
        return rUNDEF;

    if (stretchRange.isValid()) {
        if (_linear) {
            if (value < stretchRange.center()) {
                double stretchFraction = (value - stretchRange.min()) / stretchRange.distance();
                value = actualRange.min() + stretchFraction * actualRange.distance();
            }
            else {
                if (value >= stretchRange.center()) {
                    double stretchFraction = (stretchRange.max() - value) / stretchRange.distance();
                    value = actualRange.max() - stretchFraction * actualRange.distance();
                }
            }
        }
    }
    value = (value - actualRange.min()) / actualRange.distance(); // scale it between 0..1
    for (int i = 0; i < _groups.size(); ++i) {
        if (value <= _groups[i].max()) {
            double delta = _groups[i].distance();
            quint32 position = 0;
            if (_step == 0) {
                position = (value - _groups[i].min()) / delta;
            }
            else
                position = ((quint32)(value - _groups[i].min()) / _step) / ((quint32)(delta / _step));

            return NumericRange::valueAt(position, &_opacityranges[i]);
        }
    }
    return rUNDEF;
}

void ContinuousOpacityLookup::addGroup(const NumericRange &range, const NumericRange &opacityrange)
{
    if (!(range.min() >= 0 && range.max() <= 1.0)) {
        ERROR2(ERR_INVALID_INIT_FOR_2, TR("Numerical range"), "Representation");
        return;
    }
    if (_opacityranges.size() > 0) {
        if (range.min() < _groups.back().min()) {
            kernel()->issues()->log(TR("Numerical ranges for representation must be added in order"));
            return;
        }
    }
    _opacityranges.push_back(opacityrange);
    _groups.push_back(range);
}

//---------------------------------------------
ItemOpacityLookup::ItemOpacityLookup(const ItemRange *irange)
{
    if (irange) {
        for (const auto v : (*irange)) {
            _opacities[v->raw()] = 1;
        }
    }
}

void ItemOpacityLookup::setOpacity(double value, double opacity ) {
    _opacities[(quint32)value] =opacity;
}
OpacityLookup *ItemOpacityLookup::clone() const {
    ItemOpacityLookup *lookup = new ItemOpacityLookup(0);
    lookup->_opacities = _opacities;
    lookup->_cyclic = _cyclic;
    return lookup;
}

double ItemOpacityLookup::value2opacity(double index, const NumericRange&  , const NumericRange& ) const {
    if (index != iUNDEF) {
        int localIndex = index;
        if (_cyclic) {
            localIndex = localIndex % _opacities.size();
        }
        auto iter = _opacities.find(localIndex);
        if (iter != _opacities.end()) {

            double opacity = (*iter).second;
            return opacity;
        }
    }
    return rUNDEF;
}


