#include "coverage.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "stretchlimits.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("stretchlimits", StretchLimits)

StretchLimits::StretchLimits()
{

}

StretchLimits::StretchLimits(VisualAttribute *p) : VisualPropertyEditor(p, "stretchlimits", TR("Stretch Limits"), QUrl("StretchLimits.qml"))
{
}

bool StretchLimits::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if (!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    Ilwis::ICoverage cov = obj.as<Coverage>();

    IDomain dom = vpmodel()->domain();
    bool useable = dom.isValid() && dom->ilwisType() == itNUMERICDOMAIN;
    return !vpmodel()->layer()->isSupportLayer() && useable;
}



VisualPropertyEditor *StretchLimits::create(VisualAttribute *p)
{
    return new StretchLimits(p);
}

double StretchLimits::min() const {
    return vpmodel()->stretchRange().min();
}
void StretchLimits::min(double value) {
    auto  range = vpmodel()->stretchRange();
    range.min(value);
    vpmodel()->stretchRange(range);
    vpmodel()->layer()->redraw();
}
double StretchLimits::max() const {
    return vpmodel()->stretchRange().max();
}
void StretchLimits::max(double value) const {
    auto  range = vpmodel()->stretchRange();
    range.max(value);
    vpmodel()->stretchRange(range);
    vpmodel()->layer()->redraw();
}


QVariantList StretchLimits::markers() const {
    QVariantList result;
    auto  range = vpmodel()->stretchRange();
    auto range2 = vpmodel()->actualRange();
    QVariantMap mp;
    mp["position"] = 0.1; //range.min() / range2.distance();
    mp["value"] = range.min();
    mp["color"] = "blue";
    result.push_back(mp);
    mp["position"] = 0.8; // range.max() / range2.distance();
    mp["value"] = range.max();
    mp["color"] = "green";
    result.push_back(mp);

    return result;
}