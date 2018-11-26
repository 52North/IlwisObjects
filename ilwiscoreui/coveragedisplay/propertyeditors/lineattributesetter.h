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

#ifndef LINEPROPERTYSETTER_H
#define LINEPROPERTYSETTER_H


#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class LinePropertySetter : public VisualPropertyEditor
{
 Q_OBJECT

    Q_PROPERTY(bool showBoundaries READ showBoundaries WRITE setShowBoundaries NOTIFY showBoundariesChanged)
    Q_PROPERTY(QColor boundaryColor READ boundaryColor WRITE setBoundaryColor NOTIFY boundaryColorChanged)
    Q_PROPERTY(int boundarywidth READ boundarywidth WRITE setBoundarywidth NOTIFY boundarywidthChanged)
public:
    LinePropertySetter();
    LinePropertySetter(VisualAttribute *p);
    bool canUse(const IIlwisObject &obj, const QString& name) const;

    bool showBoundaries() const;
    void setShowBoundaries(bool yesno);
    QColor boundaryColor() const;
    void setBoundaryColor(const QColor& clr);
    float boundarywidth() const;
    void setBoundarywidth(float w);

    static VisualPropertyEditor *create(VisualAttribute *p);

 signals:
     void showBoundariesChanged();
     void boundaryColorChanged();
     void boundarywidthChanged();
};
}
}

#endif // LINEPROPERTYSETTER_H
