#include "controlpointslistmodel.h"

using namespace Ilwis;
using namespace Ui;

ControlPointsListModel::ControlPointsListModel(QObject *parent)
    : QObject(parent)
{
    ControlPointModel *dummy = new ControlPointModel(this);
    dummy->active(true);
    dummy->column(220);
    dummy->row(650);
    dummy->x(22.45);
    dummy->y(67.90878);
    dummy->rowError(-22.3);
    dummy->columnError(5.90);
    _controlPoints.push_back(dummy);

}

ControlPointsListModel::~ControlPointsListModel()
{
}

QQmlListProperty<ControlPointModel> ControlPointsListModel::controlPoints() {
    return QQmlListProperty<ControlPointModel>(this, _controlPoints);
}

void ControlPointsListModel::addTiepoint() {
    _controlPoints.push_back(new ControlPointModel(false, 0,0,0,0,0,0,this));
    emit controlPointsChanged();
}