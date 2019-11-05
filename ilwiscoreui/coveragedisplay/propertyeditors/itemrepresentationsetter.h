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

	Q_INVOKABLE void representationChanged(); // to inform the underlying "owner" of the representation that a change of an element has occured

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
