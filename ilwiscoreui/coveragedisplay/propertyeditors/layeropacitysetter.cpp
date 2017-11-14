#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "layeropacitysetter.h"


using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("layeropacitysetter",LayerOpacitySetter)

LayerOpacitySetter::LayerOpacitySetter()
{

}

LayerOpacitySetter::LayerOpacitySetter(VisualAttribute *p) :
    VisualPropertyEditor(p, "layeropacitysetter",TR("Opacity"),QUrl("LayerOpacity.qml"))
{

}

bool LayerOpacitySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttribute::LAYER_ONLY;

}

VisualPropertyEditor *LayerOpacitySetter::create(VisualAttribute *p)
{
    return new LayerOpacitySetter(p);
}

float LayerOpacitySetter::opacity() const
{
    if ( vpmodel()->layer())
        return vpmodel()->layer()->vproperty("opacity").toFloat();
    return 1.0;
}

void LayerOpacitySetter::opacity(float tr)
{
    if (!vpmodel()->layer())
        return;

    if ( tr >= 0 && tr <= 1.0){
       vpmodel()->layer()->vproperty("opacity", tr);
       vpmodel()->layer()->redraw();
    }
}

