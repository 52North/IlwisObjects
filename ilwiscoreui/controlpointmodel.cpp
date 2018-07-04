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
