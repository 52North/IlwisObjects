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

#ifndef ATTRIBUTEVISUALPROPERTIES_H
#define ATTRIBUTEVISUALPROPERTIES_H

#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "attributemodel.h"
#include "containerstatistics.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {

class Representation;
class Domain;
class RasterCoverage;

typedef IlwisData<Representation> IRepresentation;
typedef IlwisData<Domain> IDomain;
typedef IlwisData<RasterCoverage> IRasterCoverage;

namespace Ui{

class VisualPropertyEditor;
class LayerModel;
class RootLayerModel;

const QString LAYER_WIDE_ATTRIBUTE = "Layer Specific";

class ILWISCOREUISHARED_EXPORT VisualAttribute : public AttributeModel
{
    Q_OBJECT
public:
	Q_PROPERTY(QQmlListProperty<Ilwis::Ui::VisualPropertyEditor> propertyEditors READ visualProperties NOTIFY propertyEditorChanged)
    Q_PROPERTY(bool isAbstractAttribute READ isAbstractAttribute CONSTANT)
	Q_PROPERTY(Ilwis::Ui::LayerModel* layer READ layer CONSTANT)

	Q_INVOKABLE Ilwis::Ui::VisualPropertyEditor *visualProperty(const QString& name) const;
	Ilwis::Ui::LayerModel *layer() const;

    VisualAttribute();
    VisualAttribute(LayerModel *layer, const DataDefinition& def, const QString& attrName, const IRepresentation &rpr = IRepresentation());
    VisualAttribute(const VisualAttribute& avp);

    IRepresentation representation() const;
    void representation(const IRepresentation& rpr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    NumericRange stretchRange(bool useActual=false) const;
    void stretchRange(const NumericRange& rng);
    QColor value2color(const QVariant& var) const;
    NumericRange actualRange() const;
    void actualRange(const NumericRange& rng);

    const static QString LAYERATTRIBUTE;
    static const QString LAYER_ONLY;
    static const QString GLOBAL_ONLY;

    std::vector<QColor> colors(int size=256) const;
    std::vector<QColor> stretchedColors(int size, NumericRange & displayStretchRange) const;
    
	bool isAbstractAttribute() const;

    void addVisualPropertyEditor(VisualPropertyEditor* property);
    QQmlListProperty<Ilwis::Ui::VisualPropertyEditor> visualProperties();
signals:
	void propertyEditorChanged();
protected:

     QList<VisualPropertyEditor *> _vproperties;
	 bool _isAbstractAttribute = false;
private:
    IRepresentation _representation;
    NumericRange _stretchRange;
    NumericRange _actualRange;
//    IRasterCoverage _raster;
    LayerModel *_layer;
};

class ILWISCOREUISHARED_EXPORT LayerAttributeModel : public VisualAttribute{
public:
    LayerAttributeModel(LayerModel *parentLayer, const Ilwis::IIlwisObject &obj, const DataDefinition& def);

};

class ILWISCOREUISHARED_EXPORT GlobalAttributeModel : public VisualAttribute{
public:
    GlobalAttributeModel(const QString &label, const QString &associatedEditor,  LayerModel *parentLayer);

private:
    QString _label;
};
}
}

#endif // ATTRIBUTEVISUALPROPERTIES_H
