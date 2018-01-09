#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "mapinformationattributesetter.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("mapinfopropertyeditor",MapInformationPropertySetter)

MapInformationPropertySetter::MapInformationPropertySetter(VisualAttribute *p) :
    VisualPropertyEditor(p, "mapinfopropertyeditor",TR("Mouse over Info"),QUrl("MapinfoProperties.qml"))
{

}

MapInformationPropertySetter::MapInformationPropertySetter()
{}

bool MapInformationPropertySetter::canUse(const IIlwisObject& obj, const QString& name ) const
{
    if (!obj.isValid())
        return false;
    if(!hasType(obj->ilwisType(), itCOVERAGE))
        return false;
    return name == VisualAttribute::LAYER_ONLY && vpmodel()->layer()->layerType() != itLINELAYER;

}

VisualPropertyEditor *MapInformationPropertySetter::create(VisualAttribute *p)
{
    return new MapInformationPropertySetter(p);
}

bool MapInformationPropertySetter::showInfo() const
{
    if ( vpmodel()->layer())
        return vpmodel()->layer()->as<CoverageLayerModel>()->showInfo();
    return true;
}

void MapInformationPropertySetter::setShowInfo(bool yesno)
{
    if (!vpmodel()->layer())
        return;

    vpmodel()->layer()->as<CoverageLayerModel>()->showInfo(yesno);

}

