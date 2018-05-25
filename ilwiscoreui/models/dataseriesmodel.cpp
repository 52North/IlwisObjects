#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "modelregistry.h"
#include "tablemodel.h"
#include "chartmodel.h"
#include "dataseriesmodel.h"

using namespace Ilwis;
using namespace Ui;

QT_CHARTS_USE_NAMESPACE

DataseriesModel::DataseriesModel() {

}

DataseriesModel::DataseriesModel(ChartModel *chartModel, quint32 x_index, quint32 y_index, quint32 z_axis)
	: QObject(chartModel), _table(chartModel->table()), _xaxis(x_index), _yaxis(y_index), _zaxis(z_axis)
{
}

QString DataseriesModel::name() const {
	return _name;
}

QVariantList DataseriesModel::points() const {
	return _points;
}

bool DataseriesModel::setData() {
	_points.clear();
	_name = _table->columndefinition(_yaxis).name();	// use Y-axis for the name

	auto datadefX = _table->columndefinition(_xaxis).datadef();
	auto datadefY = _table->columndefinition(_yaxis).datadef();
	if (datadefX.domain()->ilwisType() != itNUMERICDOMAIN)
		return false;		// TODO: deal with categorical data later
	if (datadefY.domain()->ilwisType() != itNUMERICDOMAIN)
		return false;		// TODO: deal with categorical data later

	auto actualRange = datadefX.range();
	auto totalRange = _table->columndefinition(_xaxis).datadef().domain()->range();
	_minx = actualRange->as<NumericRange>()->min();
	_maxx = actualRange->as<NumericRange>()->max();

	actualRange = datadefY.range();
	totalRange = _table->columndefinition(_xaxis).datadef().domain()->range();
	_miny = actualRange->as<NumericRange>()->min();
	_maxy = actualRange->as<NumericRange>()->max();

	QVariant v;
	double vx = 0.0, vy = 0.0;
	for (int row = 0; row < _table->recordCount(); row++) {
		v = _table->cell(_xaxis, row, false);
		if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
			vx = v.toDouble();
		v = _table->cell(_yaxis, row, false);
		if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
			vy = v.toDouble();

		_points.append(QPointF(vx, vy));
	}
	emit onPointsChanged();

	return true;
}

QColor DataseriesModel::color() const {
	return _color;
}

void DataseriesModel::setColor(const QColor color) {
	_color = color;

	emit onColorChanged();
}

int DataseriesModel::xColumnIndex() const {
    return _xaxis;
}

int DataseriesModel::yColumnIndex() const {
    return _yaxis;
}

int DataseriesModel::zColumnIndex() const {
    return _zaxis;
}