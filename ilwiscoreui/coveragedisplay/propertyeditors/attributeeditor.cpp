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
#include "representationelement.h"
#include "models/visualattributemodel.h"
#include "attributeeditor.h"

VisualAttributeEditor::VisualAttributeEditor(QObject *parent) : QObject(parent)
{

}

VisualAttributeEditor::VisualAttributeEditor(const QString& name, const QString& displayNm, const QUrl &url, QObject *parent) :
    QObject(parent), Ilwis::Identity(name),
    _qmlUrl(url),
    _displayName(displayNm)
{

}

VisualAttributeEditor::VisualAttributeEditor(const VisualAttributeEditor& metadata, QObject *parent) : QObject(parent), Ilwis::Identity(metadata.name())
{
    _qmlUrl = metadata._qmlUrl;
}

QString VisualAttributeEditor::qmlUrl() const
{
    return _qmlUrl.toString();
}

QString VisualAttributeEditor::editorName() const
{
    return name();
}

QString VisualAttributeEditor::attributeName() const
{
    if ( attribute()){
        return attribute()->attributename();
    }
    return sUNDEF;
}

int VisualAttributeEditor::defaultHeight() const
{
    return 0;
}

bool VisualAttributeEditor::canUse(const IIlwisObject &, const ColumnDefinition &) const
{
    return false;
}

bool VisualAttributeEditor::canUse(const Ilwis::IIlwisObject &, const QString &) const
{
    return false;
}

void VisualAttributeEditor::prepare( VisualAttributeModel *vattrib, const IIlwisObject &, const ColumnDefinition &)
{
    _visualAttribute = vattrib;
    setParent(vattrib);
}

int VisualAttributeEditor::layerIndex() const
{
    return _layerIndex;
}

QString VisualAttributeEditor::displayName() const
{
    return _displayName;
}

void VisualAttributeEditor::setlayer(quint32 index){
    _layerIndex = index;
}

VisualAttributeModel *VisualAttributeEditor::attribute() const
{
    return _visualAttribute;
}

VisualAttributeModel *VisualAttributeEditor::attribute()
{
    return _visualAttribute;
}

void VisualAttributeEditor::attributesChanged(Raw , const QVariantMap& ){
    if ( _visualAttribute->layer()){
        _visualAttribute->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
        _visualAttribute->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
        _visualAttribute->layer()->drawer()->redraw();
    }
}

QString VisualAttributeEditor::representationName() const
{
    if ( attribute()->representation().isValid())
        return attribute()->representation()->name();
    return "";
}

void VisualAttributeEditor::representationChanged(const IRepresentation &rpr)
{
    // empty implementation
}

void VisualAttributeEditor::displayName(const QString& newname){
    _displayName = newname;
}




