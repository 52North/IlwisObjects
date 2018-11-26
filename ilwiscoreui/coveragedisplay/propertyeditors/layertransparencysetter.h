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

#ifndef LAYERTRANSPARENCYSETTER_H
#define LAYERTRANSPARENCYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class LayerTransparencySetter : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(float transparency READ transparency WRITE transparency NOTIFY transparencyChanged)
public:
    LayerTransparencySetter(QObject *parent = 0);
    ~LayerTransparencySetter();

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    float transparency() const;
    void transparency(float tr);


  NEW_PROPERTYEDITOR(LayerTransparencySetter)
signals:
      void transparencyChanged();

private:
};

#endif // LAYERTRANSPARENCYSETTER_H
