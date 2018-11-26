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

#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "representationelementmodel.h"
#include "visualpropertyeditor.h"

using namespace Ilwis;
using namespace Ui;

RepresentationElementModel::RepresentationElementModel()
{

}

RepresentationElementModel::RepresentationElementModel(VisualPropertyEditor *p) : QObject(p)
{
}

RepresentationElementModel::RepresentationElementModel(const QString &label, VisualPropertyEditor *p) : QObject(p), Identity(label)
{
}

RepresentationElementModel::RepresentationElementModel(const QString& label, double fract, VisualPropertyEditor *p) : QObject(p), Identity(label), _fraction(fract) {

}

RepresentationElementModel::RepresentationElementModel(const IRepresentation& rpr, Raw raw, const QString &label, VisualPropertyEditor *parent) : QObject(parent), Identity(label), _rpr(rpr), _raw(raw)
{
}

QColor RepresentationElementModel::color() const
{
    if (_rpr.isValid()) {
        QColor clr =  _rpr->colors()->value2color(_raw);
        clr.setAlphaF(1);
        return clr;
    }
    return QColor();
}

void RepresentationElementModel::color(const QColor &clr)
{
    if (_rpr.isValid())
        _rpr->colors()->setColor(_raw, clr);
    emit ecolorChanged();
}

double RepresentationElementModel::opacity() const {
    QColor clr;
    if (_rpr.isValid())
        clr = _rpr->colors()->value2color(_raw);
    return clr.alphaF();
}
void RepresentationElementModel::opacity(double v) {
    if (v >= 0 && v <= 1) {
        QColor clr = color();
        clr.setAlphaF(v);
        color(clr);
        emit eopacityChanged();
    }
}

double RepresentationElementModel::fraction() const {
	return _fraction;
}


