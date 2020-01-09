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

#include <algorithm>
#include <map>
#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "modelregistry.h"
#include "tablemodel.h"
#include "chartmodel.h"
#include "chartoperationfactory.h"
#include "chartoperationeditor.h"
#include "intervalrange.h"
#include "dataseriesmodel.h"

using namespace Ilwis;
using namespace Ui;

QT_CHARTS_USE_NAMESPACE

DataseriesModel::DataseriesModel(const QString name) : _name(name) {

}

DataseriesModel::DataseriesModel(ChartModel *chartModel, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QVariantMap& extra)
	: QObject(chartModel), _xaxis(xaxis), _yaxis(yaxis), _zaxis(zaxis)
{
	if (extra.contains("color"))
		_color = extra["color"].value<QColor>();
    _seriesIndex = 0;
	if (extra.contains("chartType"))
		_type = extra["chartType"].toString();
}

QString DataseriesModel::name() const {
	return _name;
}

QVariantList DataseriesModel::points() const {
	return _points;
}

void DataseriesModel::name(const QString& newName) {
	_name = newName;
	_yaxis = newName;
	emit isNameChanged();
}

bool DataseriesModel::setData(const ITable& inputTable, const QVariantMap& extra) {
    _points.clear();
    _name = inputTable->columndefinition(_yaxis).name();	// use Y-axis for the name
    _dataDefinitions[0] = inputTable->columndefinition(_xaxis).datadef();
    _dataDefinitions[1] = inputTable->columndefinition(_yaxis).datadef();
    _dataDefinitions[2] = inputTable->columndefinition(_zaxis).datadef();
    if (!_dataDefinitions[0].isValid() || !_dataDefinitions[1].isValid()) {
        return false;
    }

    auto xtype = _dataDefinitions[0].domain()->ilwisType();
	auto valueType = _dataDefinitions[0].domain()->valueType();
    if (xtype != itNUMERICDOMAIN && xtype != itITEMDOMAIN)
        return false;
    if (_dataDefinitions[1].domain()->ilwisType() != itNUMERICDOMAIN)
        return false;		// TODO: deal with categorical data later

    auto actualRangeX = _dataDefinitions[0].range();
    auto totalRangeX = _dataDefinitions[0].domain()->range();
    std::map<QString, int> lookup;
    if (xtype == itNUMERICDOMAIN) {
        _minx = extra.contains("minx") ? extra["minx"].toDouble() : actualRangeX->as<NumericRange>()->min();
        _maxx = extra.contains("maxx") ? extra["maxx"].toDouble() : actualRangeX->as<NumericRange>()->max();
    }
    else if (xtype == itITEMDOMAIN) {
        ItemRangeIterator iter(totalRangeX->as<ItemRange>());
		if (valueType == itNUMERICITEM) {
			double rmin = rUNDEF, rmax = rUNDEF;
			while (iter.isValid()) {
				SPDomainItem item = (*iter);
				Interval *interval = item->as<Interval>();
				rmin = Ilwis::min(interval->range().min(), rmin);
				rmax = Ilwis::max(interval->range().max(), rmax);
				QString label = item->name();
				++iter;
			}
			_minx = extra.contains("minx") ? extra["minx"].toDouble() : rmin;
			_maxx = extra.contains("maxx") ? extra["maxx"].toDouble() : rmax;
		}
		else {
			std::vector<int> ids;
			while (iter.isValid()) {
				SPDomainItem item = (*iter);
				QString label = item->name();
				quint32 ix = item->raw();
				lookup[label] = ix;
				ids.push_back(ix);
				++iter;
			}
			auto res = std::minmax_element(ids.begin(), ids.end());
			_minID = ids[res.first - ids.begin()];
			_maxID = ids[res.second - ids.begin()];
		}
    }

	auto actualRangeY = _dataDefinitions[1].range();
	auto totalRangeY = _dataDefinitions[1].domain()->range();
	_miny = extra.contains("miny") ? extra["miny"].toDouble() : actualRangeY->as<NumericRange>()->min();
	_maxy = extra.contains("maxy") ? extra["maxy"].toDouble() : actualRangeY->as<NumericRange>()->max();

	QVariant v;
	double vx = 0.0, vy = 0.0;
	for (int row = 0; row < inputTable->recordCount(); row++) {
		v = inputTable->cell(_xaxis, row, false);
		if (xtype == itITEMDOMAIN) {
			if (valueType == itNUMERICITEM) {
				IIntervalDomain idom = _dataDefinitions[0].domain().as<IntervalDomain>();
				auto item = idom->item(v.toString());
				vx = (item->as<Interval>()->range().min() + item->as<Interval>()->range().max()) / 2.0;
			}
			else {
				vx = lookup[v.toString()];
			}
		}

		else if (v.toDouble() != rUNDEF && v.toDouble() != iUNDEF)
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
			if (datadef.domain()->valueType() == itNUMERICITEM) {
				auto rng = datadef.range<IntervalRange>()->totalRange();
				return rng.resolution();
			}
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


DataDefinition DataseriesModel::datadefinition(ChartModel::Axis axis) const
{
    if (axis == ChartModel::Axis::AXAXIS)
        return _dataDefinitions[0];
    if (axis == ChartModel::Axis::AYAXIS)
        return _dataDefinitions[1];
    if (axis == ChartModel::Axis::AXAXIS)
        return _dataDefinitions[2];

    return DataDefinition();
}

void DataseriesModel::fillOperations() {
    ChartModel* chartModel = static_cast<ChartModel*>(parent());
    auto *factory = Ilwis::kernel()->factory<ChartOperationFactory>("ilwis::chartoperationfactory");
    if (factory) {
        QVariantMap parameters = { { "dataseries", true }, {"yaxistype", axisType(ChartModel::Axis::AYAXIS) } };
		parameters["specialtype"] = chartModel->specialType();
        _operations = factory->selectedOperations(chartModel, parameters);
        for (auto iter = _operations.begin(); iter != _operations.end(); ++iter)
            (*iter)->setParent(this);
    }
}

QQmlListProperty<ChartOperationEditor> DataseriesModel::operations()
{
    if (_operations.isEmpty())
        fillOperations();

    return QQmlListProperty<ChartOperationEditor>(this, _operations);
}

ChartOperationEditor * DataseriesModel::operation(quint32 index)
{
    if (_operations.isEmpty())
        fillOperations();

    if (index < _operations.size())
        return _operations[index];

    return nullptr;
}

QColor DataseriesModel::color() const {
	return _color;
}

QString DataseriesModel::charttype() const {
    return _type;
}

void DataseriesModel::setColor(const QColor color) {
	_color = color;

	emit onColorChanged();
}

void DataseriesModel::setType(const QString type)
{
    _type = type;

    emit onTypeChanged();
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

quint16 DataseriesModel::axisType(ChartModel::Axis at) const
{
    int ddix = (int)at - 1;
    auto xtype = _dataDefinitions[ddix].domain()->ilwisType();
	auto valueType = _dataDefinitions[ddix].domain()->valueType();
    if (xtype == itITEMDOMAIN && valueType != itNUMERICITEM)
        return static_cast<quint16>(ChartModel::AxisType::AT_CATEGORIES);

    return static_cast<quint16>(ChartModel::AxisType::AT_VALUE);      // TODO: datetime domain
}

quint16 DataseriesModel::xAxisType() const {
    return axisType(ChartModel::Axis::AXAXIS);
}

quint16 DataseriesModel::yAxisType() const {
    return axisType(ChartModel::Axis::AYAXIS);
}

QVariantMap DataseriesModel::categories(QString axis, bool unique)
{
    QVariantMap cat;

    int domix = 0;
    if (axis == "xaxis")
        domix = 0;
    else if (axis == "yaxis")
        domix = 1;
    else if (axis == "zaxis")
        domix = 2;
    else 
        return cat;

    QList<int> keys;
    QList<QString> labels;
    auto totalRange = _dataDefinitions[domix].domain()->range();
    ItemRangeIterator iter(totalRange->as<ItemRange>());
    std::map<QString, int> cats;
    while (iter.isValid()) {
        SPDomainItem item = (*iter);
        QString label = item->name();
        int ix = item->raw();
        if (unique)
            cats[label] = ix;
        else {
            keys.append(ix);
            labels.append(label);
        }

        ++iter;
    }

    QVariant vkey;
    QVariant vlabel;
    if (unique) {
        for (const auto it : cats) {
            keys.append(it.second);
            labels.append(it.first);
        }
    }
    vkey.setValue(keys);
    vlabel.setValue(labels);

    cat["keys"] = vkey;
    cat["labels"] = vlabel;
    return cat;
}
