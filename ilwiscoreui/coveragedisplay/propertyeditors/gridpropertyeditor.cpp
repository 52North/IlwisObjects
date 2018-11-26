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
        QString layername = vpmodel()->layer()->text();
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
        QString layername = vpmodel()->layer()->text();
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
SecondaryGridCellCountEditor::SecondaryGridCellCountEditor() : SubGridPropertyEditor() {

}

SecondaryGridCellCountEditor::SecondaryGridCellCountEditor(VisualAttribute * p) : SubGridPropertyEditor(p, "secondarycellcountpropertyeditor", TR("Cell Count"), QUrl("GridCellCountEditor.qml"))
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

