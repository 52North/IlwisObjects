#include "kernel.h"
#include "raster.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "applyundefinedvaluedefinition.h"

using namespace Ilwis;
using namespace Ui;



ApplyUndefinedValueDefinition::ApplyUndefinedValueDefinition()
{

}

ApplyUndefinedValueDefinition::ApplyUndefinedValueDefinition(VisualAttribute *p) :
    VisualPropertyEditor(p, "applyundefinedvalue",TR("Set Undefined value"),QUrl("UndefinedValueSetter.qml"))
{

}

bool ApplyUndefinedValueDefinition::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if ( obj->ilwisType() == itRASTER)   {
        Ilwis::IRasterCoverage raster = obj.as<RasterCoverage>();
        return hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN);
    }
    return false;
}

VisualPropertyEditor *ApplyUndefinedValueDefinition::create(VisualAttribute *p)
{
    return new ApplyUndefinedValueDefinition(p);
}

double ApplyUndefinedValueDefinition::undefinedvalue() const
{
    if ( vpmodel()->layer())
        return vpmodel()->layer()->vproperty("undefinedvalue").toFloat();
    return rUNDEF;
}

void ApplyUndefinedValueDefinition::undefinedvalue(double tr)
{
    if (!vpmodel()->layer())
        return;

    vpmodel()->layer()->vproperty("undefinedvalue", tr);
   // attribute()->layer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
   // attribute()->layer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
    vpmodel()->layer()->redraw();

}

