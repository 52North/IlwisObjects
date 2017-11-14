#ifndef NUMERICREPRESENTATIONSETTER_H
#define NUMERICREPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "coveragedisplay/propertyeditors/visualpropertyeditor.h"
#include "representationelement.h"
namespace Ilwis {
namespace Ui{
class ILWISCOREUISHARED_EXPORT NumericRepresentationSetter : public VisualPropertyEditor
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<RepresentationElement> representationElements READ representationElements NOTIFY itemsChanged)
public:
    NumericRepresentationSetter(VisualAttribute *p);
    NumericRepresentationSetter();

	bool canUse(const IIlwisObject & obj, const DataDefinition & coldef) const;

    void prepare(const IIlwisObject &, const DataDefinition &);
    static VisualPropertyEditor *create(VisualAttribute *p);

    Q_INVOKABLE QColor color(const QString &property, double frac);
    Q_INVOKABLE bool canUse(const QString& id) const;
    Q_INVOKABLE void setRepresentation(const QString& name)    ;
    QQmlListProperty<RepresentationElement> representationElements();

    NEW_PROPERTYEDITOR(NumericRepresentationSetter)

signals:
    void rprNameChanged();
    void itemsChanged();

private:

    IRepresentation _representation;
};
}
}

#endif // NUMERICREPRESENTATIONSETTER_H
