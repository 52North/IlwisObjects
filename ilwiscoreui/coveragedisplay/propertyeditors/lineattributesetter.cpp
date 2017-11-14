#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
//#include "../../models/attributesetter.h"
#include "raster.h"
#include "featurecoverage.h"
#include "representation.h"
#include "feature.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "lineattributesetter.h"


using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("linepropertysetter",LinePropertySetter)

LinePropertySetter::LinePropertySetter()
{}

LinePropertySetter::LinePropertySetter(VisualAttribute *p) :
    VisualPropertyEditor(p,"linepropertysetter",TR("Line properties"),QUrl("LineStyleSetter.qml"))
{
}

bool LinePropertySetter::canUse(const IIlwisObject &obj, const QString& name) const
{
    if ( !obj.isValid())
        return false;

    if ( hasType(obj->ilwisType(), itFEATURE)){
        IFeatureCoverage features = obj.as<FeatureCoverage>();
        return (name == VisualAttribute::LAYER_ONLY) && (features->featureCount(itLINE) || features->featureCount(itPOLYGON)> 0);
    }
    return false;
}

VisualPropertyEditor *LinePropertySetter::create(VisualAttribute *p)
{
    return new LinePropertySetter(p);
}

bool LinePropertySetter::showBoundaries() const
{
    if ( !vpmodel()->layer())
        return false;

    QVariant var = vpmodel()->layer()->vproperty("polygonboundaries");
    if ( var.isValid())
        return var.toBool();
    return false;

}

void LinePropertySetter::setShowBoundaries(bool yesno)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("polygonboundaries", yesno);
    vpmodel()->layer()->redraw();

}

void LinePropertySetter::setBoundaryColor(const QColor &clr)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("boundarycolor", clr);
  //  vproperty()->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
  //  vproperty()->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, Ilwis::IOOptions("polygononly",true));
    vpmodel()->layer()->redraw();
}

QColor LinePropertySetter::boundaryColor() const
{
    if ( vpmodel()->layer()){
        QVariant var = vpmodel()->layer()->vproperty("boundarycolor");
        if ( var.isValid())
            return var.value<QColor>();
    }
    return QColor();

}

float LinePropertySetter::boundarywidth() const
{
    if ( !vpmodel()->layer())
        return 1.0;
    QVariant var = vpmodel()->layer()->vproperty("boundarywidth");
    if ( var.isValid())
        return var.toInt();
    return 1.0;

}

void LinePropertySetter::setBoundarywidth(float w)
{
    if ( !vpmodel()->layer())
        return ;

    vpmodel()->layer()->vproperty("boundarywidth", w);
    vpmodel()->layer()->redraw();
}

