#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
#include "geometries.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "rootlayermodel.h"
#include "gridpropertyeditor.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("primarygridpropertyeditor",PrimaryGridEditor)
REGISTER_PROPERTYEDITOR("secondarygridpropertyeditor",SecondaryGridEditor)

SubGridPropertyEditor::SubGridPropertyEditor(VisualAttribute *p, const QString& editname, const QString& label,const QUrl& qml) :
    VisualPropertyEditor(p, editname,label,qml)
{

}

SubGridPropertyEditor::SubGridPropertyEditor()
{

}

bool SubGridPropertyEditor::canUse(const IIlwisObject &obj, const QString &name) const{
    bool ok = name == "gridpropertyeditor";
    return ok;
}

double SubGridPropertyEditor::opacity() const
{
    return vpmodel()->layer()->vproperty("gridlineopacity").toDouble();
}

void SubGridPropertyEditor::opacity(double v)
{
    if ( v >= 0 && v <= 1){
        vpmodel()->layer()->vproperty("gridlineopacity", v);
        vpmodel()->layer()->redraw();
    }
}

QColor SubGridPropertyEditor::lineColor() const
{
    return vpmodel()->layer()->vproperty("gridlinecolor").value<QColor>();
}

void SubGridPropertyEditor::lineColor(const QColor &clr)
{
        vpmodel()->layer()->vproperty("gridlinecolor", qVariantFromValue(clr));
        vpmodel()->layer()->redraw();
}

//-----------------------------------------

PrimaryGridEditor::PrimaryGridEditor(VisualAttribute *p) : SubGridPropertyEditor(p,"primarygridpropertyeditor",TR("Primary Grid"),QUrl("GridPropertyEditor.qml"))
{

}

PrimaryGridEditor::PrimaryGridEditor()
{

}

double PrimaryGridEditor::distance() const
{
    return vpmodel()->layer()->vproperty("gridcelldistance").toDouble() ;
}

void PrimaryGridEditor::distance(double v)
{
    if ( v > 0 ){
        vpmodel()->layer()->vproperty("gridcelldistance", v);
        vpmodel()->layer()->redraw();
    }
}

QString PrimaryGridEditor::gridDrawerName() const
{
    return "PrimaryGridDrawer";
}

void PrimaryGridEditor::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef){
    SubGridPropertyEditor::prepare(bj,datadef);
    //the create name is a generalized name, now we can set it to the specific name
    name("primarygridpropertyeditor");
    displayName(TR("Primary Grid"));

}

VisualPropertyEditor *PrimaryGridEditor::create(VisualAttribute *p){
    return new PrimaryGridEditor(p);
}

//-----------------------------------------------------
SecondaryGridEditor::SecondaryGridEditor(VisualAttribute *p) :
    SubGridPropertyEditor(p,"secondarygridpropertyeditor",TR("Secondary Grid"),QUrl("GridPropertyEditor.qml"))
{

}

SecondaryGridEditor::SecondaryGridEditor()
{

}

int SecondaryGridEditor::noOfCells() const
{
    return vpmodel()->layer()->vproperty("cellcount").toInt() ;
}

void SecondaryGridEditor::noOfCells(int v)
{
    if ( v > 0 ){
        vpmodel()->layer()->vproperty("cellcount", v);
        vpmodel()->layer()->redraw();
    }
}

QString SecondaryGridEditor::gridDrawerName() const
{
    return "SecondaryGridDrawer";
}

VisualPropertyEditor *SecondaryGridEditor::create(VisualAttribute *p)
{
    return new SecondaryGridEditor(p);
}

void SecondaryGridEditor::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef){
    SubGridPropertyEditor::prepare(bj,datadef);
    //the create name is a generalized name, now we can set it to the specific name
    name("secondarygridpropertyeditor");
    displayName(TR("Secondary Grid"));

}



