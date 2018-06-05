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

DataseriesModel::DataseriesModel(const QString name) : _name(name) {

}

DataseriesModel::DataseriesModel(ChartModel *chartModel, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QColor& color)
	: QObject(chartModel), _xaxis(xaxis), _yaxis(yaxis), _zaxis(zaxis), _color(color)
{
}

QString DataseriesModel::name() const {
	return _name;
}

QVariantList DataseriesModel::points() const {
	return _points;
}

bool DataseriesModel::setData(const ITable& inputTable) {
	_points.clear();
	_name = inputTable->columndefinition(_yaxis).name();	// use Y-axis for the name
    _dataDefinitions[0] = inputTable->columndefinition(_xaxis).datadef();
    _dataDefinitions[1] = inputTable->columndefinition(_yaxis).datadef();
    _dataDefinitions[2] = inputTable->columndefinition(_zaxis).datadef();
    if (!_dataDefinitions[0].isValid() || !_dataDefinitions[1].isValid()) {
        return false;
    }

	if (_dataDefinitions[0].domain()->ilwisType() != itNUMERICDOMAIN)
		return false;		// TODO: deal with categorical data later
	if (_dataDefinitions[1].domain()->ilwisType() != itNUMERICDOMAIN)
		return false;		// TODO: deal with categorical data later

	auto actualRange = _dataDefinitions[0].range();
	auto totalRange = _dataDefinitions[0].domain()->range();
	_minx = actualRange->as<NumericRange>()->min();
	_maxx = actualRange->as<NumericRange>()->max();

	actualRange = _dataDefinitions[1].range();
	totalRange = _dataDefinitions[1].domain()->range();
	_miny = actualRange->as<NumericRange>()->min();
	_maxy = actualRange->as<NumericRange>()->max();

	QVariant v;
	double vx = 0.0, vy = 0.0;
	for (int row = 0; row < inputTable->recordCount(); row++) {
		v = inputTable->cell(_xaxis, row, false);
		if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
			vx = v.toDouble();
		v = inputTable->cell(_yaxis, row, false);
		if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
			vy = v.toDouble();

		_points.append(QPointF(vx, vy));
	}
	emit onPointsChanged();

	return true;
}

int tickResolution(const DataDefinition& datadef) {
    if (datadef.isValid()) {
        if (datadef.domain()->ilwisType() == itNUMERICDOMAIN)
            return datadef.domain()->range<NumericRange>()->resolution();
        if (datadef.domain()->ilwisType() == itITEMDOMAIN)
            return 1;
    }
    return 0;
}
double DataseriesModel::resolutionX()
{
    if (_xaxis != sUNDEF) {
        return tickResolution(_dataDefinitions[0]);

    }
    return 0;
}

double DataseriesModel::resolutionY()
{
    if (_yaxis != sUNDEF) {
        return tickResolution(_dataDefinitions[1]);

    }
    return 0;
}

double DataseriesModel::resolutionZ()
{
    if (_zaxis != sUNDEF) {
        return tickResolution(_dataDefinitions[2]);
 
    }
    return 0;
}


DataDefinition Ilwis::Ui::DataseriesModel::datadefinition(ChartModel::Axis axis)
{
    if (axis == ChartModel::aXAXIS)
        return _dataDefinitions[0];
    if (axis == ChartModel::aYAXIS)
        return _dataDefinitions[1];
    if (axis == ChartModel::aXAXIS)
        return _dataDefinitions[2];

    return DataDefinition();

QQmlListProperty<ChartOperation> Ilwis::Ui::DataseriesModel::operations()
{
    return QQmlListProperty<ChartOperation>();
}

Q_INVOKABLE Ilwis::Ui::ChartOperation * Ilwis::Ui::DataseriesModel::operation(quint32 index)
{
    return nullptr;
}

QColor DataseriesModel::color() const {
	return _color;
}

void DataseriesModel::setColor(const QColor color) {
	_color = color;

	emit onColorChanged();
}

QString DataseriesModel::xColumn() const {
    return _xaxis;
}

QString DataseriesModel::yColumn() const {
    return _yaxis;
}

QString DataseriesModel::zColumn() const {
    return _zaxis;
}