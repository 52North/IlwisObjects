#include "controlpointmodel.h"

using namespace Ilwis;
using namespace Ui;

ControlPointModel::ControlPointModel(QObject *parent)
    : QObject(parent)
{
}

ControlPointModel::ControlPointModel(bool act, double px, double py, quint32 pcolumn, quint32 prow, double pcolumnError, double prowError, QObject *parent) {
    active(act);
    x(px);
    y(py);
    column(pcolumn);
    row(prow);
    columnError(pcolumnError);
    rowError(prowError);
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

quint32 ControlPointModel::row() const {
    return _controlPoint.gridLocation().y;
}
void ControlPointModel::row(quint32 r) {
    _controlPoint.gridLocationRef().y = r;
}

quint32 ControlPointModel::column() const {
    return _controlPoint.gridLocation().x;
}
void ControlPointModel::column(quint32 c) {
    _controlPoint.gridLocationRef().x = c;
}

double ControlPointModel::rowError() const {
    return _controlPoint.errorRow();
}
void ControlPointModel::rowError(double value) {
    _controlPoint.errorRow(value);
}

double ControlPointModel::columnError() const {
    return _controlPoint.errorColumn();
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
