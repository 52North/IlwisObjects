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

#ifndef CONVERTATTRIBUTEDOMAIN_H
#define CONVERTATTRIBUTEDOMAIN_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class ILWISCOREUISHARED_EXPORT ConvertAttributeDomain : public VisualPropertyEditor
{
        Q_OBJECT

    Q_PROPERTY(QStringList colorSchemes READ colorSchemes CONSTANT)

public:
    ConvertAttributeDomain();
    ConvertAttributeDomain(VisualAttribute *attr);
    bool canUse(const IIlwisObject &obj, const DataDefinition &) const;
    static VisualPropertyEditor *create(VisualAttribute *);

    Q_INVOKABLE bool execute(const QString& colorScheme, const QString& viewerId);
    Q_INVOKABLE QStringList allowedDomains() const;
    Q_INVOKABLE int domainTypeIndex();

    void prepare(const IIlwisObject &obj, const DataDefinition &datadef);
private:
    QStringList colorSchemes() const;
    DataDefinition _attributeDef;

};
}
}

#endif // CONVERTATTRIBUTEDOMAIN_H
