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
	void representationChanged(const IRepresentation &rpr);
    QQmlListProperty< Ilwis::Ui::RepresentationElementModel > representationElements();
signals:
    void itemsChanged();

private:
	void fillElements();
};
}
}

#endif // NUMERICREPRESENTATIONSETTER_H
