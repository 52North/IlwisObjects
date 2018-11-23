#ifndef NUMERICREPRESENTATIONSETTER_H
#define NUMERICREPRESENTATIONSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "coveragedisplay/propertyeditors/visualpropertyeditor.h"
#include "representationelementmodel.h"
namespace Ilwis {
namespace Ui{
class ILWISCOREUISHARED_EXPORT NumericRepresentationSetter : public VisualPropertyEditor
{
    Q_OBJECT

  
public:
	Q_PROPERTY(QQmlListProperty<Ilwis::Ui::RepresentationElementModel> representationElements READ representationElements NOTIFY itemsChanged)

    NumericRepresentationSetter(VisualAttribute *p);
    NumericRepresentationSetter();

	bool canUse(const IIlwisObject & obj, const DataDefinition & coldef) const;
	bool canUse(const IIlwisObject &obj, const QString& name) const;

    void prepare(const IIlwisObject &, const DataDefinition &);
    static VisualPropertyEditor *create(VisualAttribute *p);

    Q_INVOKABLE QColor color(const QString &property, double frac);
    Q_INVOKABLE bool canUse(const QString& id) const;
    Q_INVOKABLE void setRepresentation(const QString& name)    ;
    QQmlListProperty< Ilwis::Ui::RepresentationElementModel > representationElements();
signals:
    void rprNameChanged();
    void itemsChanged();

private:
	void fillElements();
};
}
}

#endif // NUMERICREPRESENTATIONSETTER_H
