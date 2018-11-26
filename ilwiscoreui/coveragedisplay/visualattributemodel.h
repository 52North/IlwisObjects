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

#ifndef VISUALATTRIBUTEMODEL_H
#define VISUALATTRIBUTEMODEL_H

#include <QQmlListProperty>
#include "attributemodel.h"
#include "ilwiscoreui_global.h"

class GlobalLayerModel;
namespace Ilwis{
class Representation;
typedef IlwisData<Representation> IRepresentation;

}
class VisualAttributeEditor;
class CoverageLayerModel;

class ILWISCOREUISHARED_EXPORT VisualAttributeModel : public AttributeModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<VisualAttributeEditor> propertyEditors READ propertyEditors NOTIFY propertyEditorChanged)

public:
    VisualAttributeModel();
    ~VisualAttributeModel();

    VisualAttributeModel(const Ilwis::ColumnDefinition& def, CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& obj);
    Ilwis::IRepresentation representation() const;
    void representation(const Ilwis::IRepresentation& rpr);
    CoverageLayerModel* layer() const;
    CoverageLayerModel* layer();

    Q_INVOKABLE VisualAttributeEditor *propertyEditor(const QString &name);

    static const QString LAYER_ONLY;
    static const QString GLOBAL_ONLY;

signals:
    void propertyEditorChanged();

protected:
    QList<VisualAttributeEditor *> _propertyEditors;

private:
    QQmlListProperty<VisualAttributeEditor> propertyEditors();
    Ilwis::IRepresentation _representation;
    CoverageLayerModel *_layer = 0;

};

class LayerAttributeModel : public VisualAttributeModel{
public:
    LayerAttributeModel(CoverageLayerModel *parentLayer, const Ilwis::IIlwisObject& obj);
    QString attributename() const;

};

class GlobalAttributeModel : public VisualAttributeModel{
public:
    GlobalAttributeModel(const QString& label, const QString& associatedEditor, GlobalLayerModel *parentLayer);
    QString attributename() const;

private:
    QString _label;
};

#endif // VISUALATTRIBUTEMODEL_H
