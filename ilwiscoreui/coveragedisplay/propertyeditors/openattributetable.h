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

#ifndef OPENATTRIBUTETABLE_H
#define OPENATTRIBUTETABLE_H


#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class OpenAttributeTable : public VisualPropertyEditor
{
    Q_OBJECT

    Q_PROPERTY(QStringList attributeTable READ attributeTable CONSTANT)

public:
    OpenAttributeTable();
    OpenAttributeTable(VisualAttribute *p);

    bool canUse(const IIlwisObject &obj, const QString &name) const;

    QStringList attributeTable() const;

    static VisualPropertyEditor *create(VisualAttribute *p);



private:
};
}
}

#endif // OPENATTRIBUTETABLE_H
