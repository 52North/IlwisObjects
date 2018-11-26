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

#ifndef ATTRIBUTESETTER_H
#define ATTRIBUTESETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class AttributeSetter : public VisualAttributeEditor
{
    Q_OBJECT
    Q_PROPERTY(QStringList attributes READ attributes NOTIFY attributesChanged)

public:
    AttributeSetter(QObject *parent=0);
    ~AttributeSetter();

    bool canUse(const Ilwis::IIlwisObject& obj) const;
    QStringList attributes();
    void prepare(const IIlwisObject &obj);

    NEW_PROPERTYEDITOR(AttributeSetter)

    static VisualAttributeEditor *create();
signals:
    void attributesChanged();

private:
     QList<VisualAttributeEditor *> _propertyEditors;
     QStringList _attributes;
};

#endif // ATTRIBUTESETTER_H
