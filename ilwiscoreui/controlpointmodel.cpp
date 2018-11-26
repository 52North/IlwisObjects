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

#include "controlpointmodel.h"
#include "geometries.h"
#include "georeference.h"
#include "layermanager.h"

using namespace Ilwis;
using namespace Ui;

ControlPointModel::ControlPointModel(QObject *parent)
    : QObject(parent)
{
}

ControlPointModel::ControlPointModel(LayerManager *manager, const QString lab, bool act, double px, double py, quint32 pcolumn, quint32 prow, double pcolumnError, double prowError, QObject *parent) {
    active(act);
    x(px);
    y(py);
    column(pcolumn);
    row(prow);
    columnError(pcolumnError);
    rowError(prowError);
    label(lab);
    _layerManager = manager;

}

ControlPointModel::~ControlPointModel()
{
}

double ControlPointModel::x() const {
    return _controlPoint.x;
}

void ControlPointModel::x(double value) {
    _controlPoint.x = value;
}
double ControlPointModel::y() const {
    return _controlPoint.y;
}
void ControlPointModel::y(double value) {
    _controlPoint.y = value;
}

double ControlPointModel::row() const {
    return _controlPoint.gridLocation().y;
}
void ControlPointModel::row(double r) {
    _controlPoint.gridLocationRef().y = r;
}

double ControlPointModel::column() const {
    return _controlPoint.gridLocation().x;
}
void ControlPointModel::column(double c) {
    _controlPoint.gridLocationRef().x = c;
}

int ControlPointModel::rowScreen() const {
    if (!_layerManager)
        return iUNDEF;

    Pixel pix = _layerManager->rootLayer()->screenGrf()->coord2Pixel(_screenCrd);
    return pix.y;
}

int ControlPointModel::columnScreen() const {
    if (!_layerManager)
        return iUNDEF;
    Pixel pix = _layerManager->rootLayer()->screenGrf()->coord2Pixel(_screenCrd);
    return pix.x;
}
void ControlPointModel::screenPosition(double c, double r) {
    if (!_layerManager)
        return ;
    Coordinate crd = _layerManager->rootLayer()->screenGrf()->pixel2Coord(Pixel(c, r));
    _screenCrd = crd;
}

double ControlPointModel::rowError() const {
    double v = _controlPoint.errorRow();
    if (v == rUNDEF)
        return -1000000;
    return v;
}
void ControlPointModel::rowError(double value) {
    _controlPoint.errorRow(value);
}

double ControlPointModel::columnError() const {
    double v = _controlPoint.errorColumn();
    if (v == rUNDEF)
        return -1000000;
    return v;
}
void ControlPointModel::columnError(double value) {
    _controlPoint.errorColumn(value);
}
bool ControlPointModel::active() const {
    return _controlPoint.isActive();
}
void ControlPointModel::active(bool yesno) {
    _controlPoint.active(yesno);
}

QString ControlPointModel::label() const {
    return _label;
}
void ControlPointModel::label(const QString& l) {
    _label = l;
    update();
}

ControlPoint ControlPointModel::controlPoint() {
    return _controlPoint;
}

void Ilwis::Ui::ControlPointModel::update()
{
    emit updateControlPoint();
}

QString ControlPointModel::color() const {
    return _color.name();
}
void ControlPointModel::color(const QColor& clr) {
    _color = clr;
}

bool Ilwis::Ui::ControlPointModel::isValid() const
{
    return x() != 0 && y() != 0 && _screenCrd.isValid();
}
