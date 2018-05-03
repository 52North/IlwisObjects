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
	setXYData();
}

QString DataseriesModel::name() const {
	return _name;
}

QVariantList DataseriesModel::points() const {
	return _points;
}

void DataseriesModel::setXYData() {
	_points.clear();
	_name = _table->columndefinition(_yaxis).name();	// use Y-axis for the name
	QVariant v;
	double vx = 0.0, vy = 0.0;
	for (int row = 0; row < _table->recordCount(); row++) {
		v = _table->cell(_xaxis, row, false);
		if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
			vx = v.toDouble();
		v = _table->cell(_yaxis, row, false);
		if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
			vy = v.toDouble();

		if (_minx == rUNDEF) {		// assume all or none are undef
			_minx = vx;
			_maxx = vx;
			_miny = vy;
			_maxy = vy;
		}
		else {
			_minx = std::min(_minx, vx);
			_maxx = std::max(_maxx, vx);
			_miny = std::min(_miny, vy);
			_maxy = std::max(_maxy, vy);
		}

		_points.append(QPointF(vx, vy));
	}
	emit onPointsChanged();
}

QColor DataseriesModel::color() const {
	return _color;
}

void DataseriesModel::setColor(const QColor color) {
	_color = color;

	emit onColorChanged();
}