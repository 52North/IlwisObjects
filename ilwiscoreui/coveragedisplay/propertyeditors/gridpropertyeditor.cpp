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

//REGISTER_PROPERTYEDITOR("primarygridpropertyeditor",PrimaryGridEditor)
//REGISTER_PROPERTYEDITOR("secondarygridpropertyeditor",SecondaryGridEditor)

SubGridPropertyEditor::SubGridPropertyEditor(VisualAttribute *p, const QString& editname, const QString& label,const QUrl& qml) :
    VisualPropertyEditor(p, editname,label,qml)
{

}

SubGridPropertyEditor::SubGridPropertyEditor()
{

}

bool SubGridPropertyEditor::canUse(const IIlwisObject &obj, const QString &name) const{
    bool ok = name == "primarygridpropertyeditor" || name == "secondarygridpropertyeditor";
    return ok;
}

double SubGridPropertyEditor::opacity() const
{
    return vpmodel()->layer()->vproperty("opacity").toDouble();
}

void SubGridPropertyEditor::opacity(double v)
{
    if ( v >= 0 && v <= 1){
        vpmodel()->layer()->vproperty("opacity", v);
        vpmodel()->layer()->redraw();
    }
}

QColor SubGridPropertyEditor::lineColor() const
{
    return vpmodel()->layer()->vproperty("fixedlinecolor").value<QColor>();
}

void SubGridPropertyEditor::lineColor(const QColor &clr)
{
        vpmodel()->layer()->vproperty("fixedlinecolor", qVariantFromValue(clr));
        vpmodel()->layer()->redraw();
}

bool SubGridPropertyEditor::isActive() const {
    const RootLayerModel *globalLayer = vpmodel()->layer()->layerManager()->rootLayer();
    if (globalLayer) {
        QString layername = vpmodel()->layer()->name();
        const LayerModel *gridLayer = globalLayer->findLayerByName(layername);
        if (gridLayer)
            return gridLayer->active();
    }
    return false;
}

void SubGridPropertyEditor::isActive(bool yesno)
{
    RootLayerModel *globalLayer = vpmodel()->layer()->layerManager()->rootLayer();
    if (globalLayer) {
        QString layername = vpmodel()->layer()->name();
        LayerModel *gridLayer = globalLayer->findLayerByName(layername);
        if (gridLayer) {
            gridLayer->active(yesno);
            gridLayer->redraw();
        }
    }
}




/*bool SecondaryGridEditor::isActive() const
{
    RootLayerModel *globalLayer = vpmodel()->layer()->layerManager()->rootLayer();
    if (globalLayer) {
        LayerModel *primaryGridLayer = globalLayer->findLayerByName("Primary Grid");
        if (primaryGridLayer) {
            LayerModel *secondaryGridLayer = globalLayer->findLayerByName("Secondary Grid");
            if (secondaryGridLayer) {
                return secondaryGridLayer->active() && primaryGridLayer->active();
            }
       }
    }
    return false;
}*/

//---------------------------------------------
REGISTER_PROPERTYEDITOR("gridlinecolorpropertyeditor", GridLineColorEditor)

GridLineColorEditor::GridLineColorEditor() : SubGridPropertyEditor() {

}

GridLineColorEditor::GridLineColorEditor(VisualAttribute * p) : SubGridPropertyEditor(p, "gridlinecolorpropertyeditor", TR("Line Color"), QUrl("GridLineColorEditor.qml"))
{
}

QString Ilwis::Ui::GridLineColorEditor::gridDrawerName() const
{
    return "gridlinecolorpropertyeditor";
}

VisualPropertyEditor *GridLineColorEditor::create(VisualAttribute *p)
{
    return new GridLineColorEditor(p);
}

//---------------------------------------------
REGISTER_PROPERTYEDITOR("primarygridopacitypropertyeditor", GridOpacityEditor)

GridOpacityEditor::GridOpacityEditor() : SubGridPropertyEditor() {

}

GridOpacityEditor::GridOpacityEditor(VisualAttribute * p) : SubGridPropertyEditor(p, "gridopacitypropertyeditor", TR("Line Opacity"), QUrl("GridLineOpacityEditor.qml"))
{
}

QString Ilwis::Ui::GridOpacityEditor::gridDrawerName() const
{
    return "gridopacitypropertyeditor";
}

VisualPropertyEditor *GridOpacityEditor::create(VisualAttribute *p)
{
    return new GridOpacityEditor(p);
}

//------------------------------------------------
REGISTER_PROPERTYEDITOR("primarygridcelldistancepropertyeditor", PrimaryGridCellDistanceEditor)

PrimaryGridCellDistanceEditor::PrimaryGridCellDistanceEditor() : SubGridPropertyEditor() {

}

PrimaryGridCellDistanceEditor::PrimaryGridCellDistanceEditor(VisualAttribute * p) : SubGridPropertyEditor(p, "primarygridcelldistancepropertyeditor", TR("Distance"), QUrl("GridLineDistanceEditor.qml"))
{
}

bool PrimaryGridCellDistanceEditor::canUse(const IIlwisObject & obj, const QString & name) const
{
    bool ok = name == "primarygridpropertyeditor";

    return ok;
}

QString Ilwis::Ui::PrimaryGridCellDistanceEditor::gridDrawerName() const
{
    return "primarygridcelldistancepropertyeditor";
}

VisualPropertyEditor *PrimaryGridCellDistanceEditor::create(VisualAttribute *p)
{
    return new PrimaryGridCellDistanceEditor(p);
}

double PrimaryGridCellDistanceEditor::distance() const
{
    return vpmodel()->layer()->vproperty("gridcelldistance").toDouble();
}

void PrimaryGridCellDistanceEditor::distance(double v)
{
    if (v > 0) {
        vpmodel()->layer()->vproperty("gridcelldistance", v);
        vpmodel()->layer()->redraw();
    }
}

//------------------------------------------------
REGISTER_PROPERTYEDITOR("secondarycellcountpropertyeditor", SecondaryGridCellCountEditor)

SecondaryGridCellCountEditor::SecondaryGridCellCountEditor() : SubGridPropertyEditor() {

}

SecondaryGridCellCountEditor::SecondaryGridCellCountEditor(VisualAttribute * p) : SubGridPropertyEditor(p, "secondarycellcountpropertyeditor", TR("Cell Count"), QUrl("GridPropertyEditor.qml"))
{
}

bool SecondaryGridCellCountEditor::canUse(const IIlwisObject & obj, const QString & name) const
{
    bool ok = name == "secondarygridpropertyeditor";

    return ok;
}

QString SecondaryGridCellCountEditor::gridDrawerName() const
{
    return "secondarycellcountpropertyeditor";
}

VisualPropertyEditor *SecondaryGridCellCountEditor::create(VisualAttribute *p)
{
    return new SecondaryGridCellCountEditor(p);
}

int SecondaryGridCellCountEditor::noOfCells() const
{
    return vpmodel()->layer()->vproperty("cellcount").toInt();
}

void SecondaryGridCellCountEditor::noOfCells(int v)
{
    if (v > 0) {
        vpmodel()->layer()->vproperty("cellcount", v);
        vpmodel()->layer()->redraw();
    }
}

