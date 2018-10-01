#include <algorithm>
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
#include "dataseriesmodel.h"

using namespace Ilwis;
using namespace Ui;

QT_CHARTS_USE_NAMESPACE

DataseriesModel::DataseriesModel(const QString name) : _name(name) {

}

DataseriesModel::DataseriesModel(ChartModel *chartModel, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QColor& color)
	: QObject(chartModel), _xaxis(xaxis), _yaxis(yaxis), _zaxis(zaxis), _color(color)
{
    auto *factory = Ilwis::kernel()->factory<ChartOperationFactory>("ilwis::chartoperationfactory");
    if (factory) {
        QVariantMap parameters = { { "dataseries", true } };
        _operations = factory->selectedOperations(chartModel, parameters);
        for (auto iter = _operations.begin(); iter != _operations.end(); ++iter)
            (*iter)->setParent(this);
    }
    _seriesIndex = 0;
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

    auto xtype = _dataDefinitions[0].domain()->ilwisType();
    if (xtype != itNUMERICDOMAIN && xtype != itITEMDOMAIN)
        return false;
    if (_dataDefinitions[1].domain()->ilwisType() != itNUMERICDOMAIN)
        return false;		// TODO: deal with categorical data later

    auto actualRangeX = _dataDefinitions[0].range();
    auto totalRangeX = _dataDefinitions[0].domain()->range();
    std::map<QString, int> lookup;
    if (xtype == itNUMERICDOMAIN) {
        _minx = actualRangeX->as<NumericRange>()->min();
        _maxx = actualRangeX->as<NumericRange>()->max();
    }
    else if (xtype == itITEMDOMAIN) {
        ItemRangeIterator iter(totalRangeX->as<ItemRange>());
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

	auto actualRangeY = _dataDefinitions[1].range();
	auto totalRangeY = _dataDefinitions[1].domain()->range();
	_miny = actualRangeY->as<NumericRange>()->min();
	_maxy = actualRangeY->as<NumericRange>()->max();

	QVariant v;
	double vx = 0.0, vy = 0.0;
	for (int row = 0; row < inputTable->recordCount(); row++) {
		v = inputTable->cell(_xaxis, row, false);
        if (xtype == itITEMDOMAIN)
            vx = lookup[v.toString()];
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


DataDefinition DataseriesModel::datadefinition(ChartModel::Axis axis)
{
    if (axis == ChartModel::AXAXIS)
        return _dataDefinitions[0];
    if (axis == ChartModel::AYAXIS)
        return _dataDefinitions[1];
    if (axis == ChartModel::AXAXIS)
        return _dataDefinitions[2];

    return DataDefinition();
}

QQmlListProperty<ChartOperationEditor> DataseriesModel::operations()
{
    return QQmlListProperty<ChartOperationEditor>(this, _operations);
}

ChartOperationEditor * DataseriesModel::operation(quint32 index)
{
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

quint16 DataseriesModel::xAxisType() const
{
    auto xtype = _dataDefinitions[0].domain()->ilwisType();
    if (xtype == itITEMDOMAIN)
        return ChartModel::AxisType::AT_CATEGORIES;

    return ChartModel::AxisType::AT_VALUE;      // TODO: datetime domain
}

QVariantMap DataseriesModel::categories(QString axis)
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
    while (iter.isValid()) {
        SPDomainItem item = (*iter);
        QString label = item->name();
        int ix = item->raw();
        keys.append(ix);
        labels.append(label);

        ++iter;
    }

    QVariant vkey;
    QVariant vlabel;
    vkey.setValue(keys);
    vlabel.setValue(labels);
    cat["keys"] = vkey;
    cat["labels"] = vlabel;
    return cat;
}
