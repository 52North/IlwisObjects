#ifndef ITEMREPRESENTATIONSETTER_H
#define ITEMREPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "coveragedisplay/propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class RepresentationElementModel;

class ILWISCOREUISHARED_EXPORT ItemRepresentationSetter : public VisualPropertyEditor
{
    Q_OBJECT


    Q_PROPERTY(QQmlListProperty<Ilwis::Ui::RepresentationElementModel> representationElements READ representationElements NOTIFY itemsChanged)
public:
    ItemRepresentationSetter();
    ItemRepresentationSetter(VisualAttribute *p);

    void prepare(const IIlwisObject &obj, const DataDefinition &coldef);
    bool canUse(const IIlwisObject &obj, const DataDefinition &) const;
    static VisualPropertyEditor *create(VisualAttribute *p);

    QQmlListProperty<RepresentationElementModel> representationElements();
    void attributesChanged(Raw index, const QVariantMap &values);
     void representationChanged(const IRepresentation& rpr);

signals:
    void rprNameChanged();
    void itemsChanged();
};
}
}

#endif // ITEMREPRESENTATIONSETTER_H
