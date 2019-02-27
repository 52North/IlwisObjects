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

#ifndef REPRESENTATIONELEMENT_H
#define REPRESENTATIONELEMENT_H

#include <QObject>
#include <QColor>
#include "identity.h"
#include "ilwiscoreui_global.h"
#include "ilwisdata.h"
#include "representation.h"

namespace Ilwis {

namespace Ui{
class VisualPropertyEditor;

class ILWISCOREUISHARED_EXPORT RepresentationElementModel : public QObject, public Ilwis::Identity{
    Q_OBJECT

public:
    RepresentationElementModel();
    explicit RepresentationElementModel(QObject *p);
    RepresentationElementModel(const QString& label, QObject *parent=0);
	RepresentationElementModel(const QString& label, double fract, QObject *parent);
    RepresentationElementModel(const IRepresentation& rpr, Raw raw,const QString& label, QObject *parent=0);
    Q_PROPERTY(QColor ecolor READ color WRITE color NOTIFY ecolorChanged)
    Q_PROPERTY(QString label READ name CONSTANT)
    Q_PROPERTY(double eopacity READ opacity NOTIFY eopacityChanged)
	Q_PROPERTY(double fraction READ fraction CONSTANT)

    QColor color() const;
    void color(const QColor& clr);
    double opacity() const;
    void opacity(double v);
	double fraction() const;

signals:
    void ecolorChanged();
    void eopacityChanged();
    void attributeChanged(int index, const QVariantMap& values);

private:
    IRepresentation _rpr;
    Raw _raw;
	double _fraction = rUNDEF;
};
}
}

#endif // REPRESENTATIONELEMENT_H
