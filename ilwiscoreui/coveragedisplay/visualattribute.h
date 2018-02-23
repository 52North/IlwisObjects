#ifndef ATTRIBUTEVISUALPROPERTIES_H
#define ATTRIBUTEVISUALPROPERTIES_H

#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "attributemodel.h"
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

class ILWISCOREUISHARED_EXPORT VisualAttribute : public AttributeModel
{
    Q_OBJECT
public:
	Q_PROPERTY(QQmlListProperty<Ilwis::Ui::VisualPropertyEditor> propertyEditors READ visualProperties NOTIFY propertyEditorChanged)
    Q_PROPERTY(bool isAbstractAttribute READ isAbstractAttribute CONSTANT)

	Q_INVOKABLE Ilwis::Ui::VisualPropertyEditor *visualProperty(const QString& name) const;

    VisualAttribute();
    VisualAttribute(LayerModel *layer, const DataDefinition& def, const QString& attrName, const IRepresentation &rpr = IRepresentation());
    VisualAttribute(const VisualAttribute& avp);

    IRepresentation representation() const;
    void representation(const IRepresentation& rpr);
    IDomain domain() const;
    void domain(const IDomain& dom);
    NumericRange stretchRange() const;
    void stretchRange(const NumericRange& rng);
    QColor value2color(const QVariant& var) const;
    NumericRange actualRange() const;
    void actualRange(const NumericRange& rng);

    const static QString LAYERATTRIBUTE;
    static const QString LAYER_ONLY;
    static const QString GLOBAL_ONLY;

    std::vector<QColor> colors(int size=256) const;
    LayerModel *layer() const;
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
