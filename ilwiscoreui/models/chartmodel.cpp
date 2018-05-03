#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "modelregistry.h"
#include "tablemodel.h"
#include "chartmodel.h"
#include "dataseriesmodel.h"
#include "mathhelper.h"

using namespace Ilwis;
using namespace Ui;

ChartModel::ChartModel() : QObject()
{
}

ChartModel::ChartModel(QObject *parent) : QObject(parent)
{
    _modelId = modelregistry()->newModelId();
    modelregistry()->registerModel(modelId(), "chart", this);
}

quint32 Ilwis::Ui::ChartModel::createChart(const QString& name, const ITable & tbl, const QString & cType, quint32 xaxis, quint32 yaxis, quint32 zaxis)
{
    _table = tbl;
    _name = name;
    chartType(cType);

	_series = QList<DataseriesModel *>();
	addDataSeries(xaxis, yaxis, zaxis);		// add the first dataseries

    return modelId();
}

QVariantList ChartModel::linkProperties() const
{
    return QVariantList();
}

quint32 ChartModel::modelId() const
{
    return _modelId;
}

QString ChartModel::name() const
{
    return _name;
}

QString Ilwis::Ui::ChartModel::mainPanelUrl()
{
    return "../../extensions/ui/Charts/qml/ChartsPanel.qml";
}

void Ilwis::Ui::ChartModel::chartType(const QString & tp)
{
    _chartType = tp;
    emit chartTypeChanged();
}

QString ChartModel::chartType() const
{
    return _chartType;
}

int ChartModel::seriesCount() const {
	return _series.size();
}

DataseriesModel* ChartModel::getSeries(int seriesIndex) const {
	if (seriesIndex >= 0 && seriesIndex < _series.size())
		return _series.at(seriesIndex);

	return NULL;
}

Ilwis::ITable ChartModel::table() const {
	return _table;
}

bool Ilwis::Ui::ChartModel::isValidSeries(const QString columnName) const
{
	return _table->columndefinitionRef(columnName).isValid();
}

quint32 Ilwis::Ui::ChartModel::addDataSeries(quint32 xaxis, quint32 yaxis, quint32 zaxis) {

	auto newseries = new DataseriesModel(this, xaxis, yaxis, zaxis);
	_series.push_back(newseries);

	if (_minx == rUNDEF) {		// assume all or none are undef
		_minx = newseries->minx();
		_maxx = newseries->maxx();
		_miny = newseries->miny();
		_maxy = newseries->maxy();
	}
	else {
		_minx = std::min(_minx, newseries->minx());
		_maxx = std::max(_maxx, newseries->maxx());
		_miny = std::min(_miny, newseries->miny());
		_maxy = std::max(_maxy, newseries->maxy());
	}

	emit xAxisChanged();
	emit yAxisChanged();
	emit chartModelChanged();

	return _series.size();
}


