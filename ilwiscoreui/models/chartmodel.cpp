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

QT_CHARTS_USE_NAMESPACE

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
DataseriesModel* ChartModel::getSeries(int xColumnIndex, int yColumnIndex, int zColumnIndex) const {
    for (auto *series : _series) {
        if (series->xColumnIndex() == xColumnIndex && series->yColumnIndex() == yColumnIndex && series->zColumnIndex() == zColumnIndex)
            return series;
    }
    return 0;
}

void ChartModel::deleteSerie(int xColumnIndex, int yColumnIndex, int zColumnIndex)  {
    for (int i = 0; i < _series.size(); ++i) {
        auto *series = _series[i];
        if (series->xColumnIndex() == xColumnIndex && series->yColumnIndex() == yColumnIndex && series->zColumnIndex() == zColumnIndex) {
            _series.removeAt(i);
            break;
        }
    }
}

DataseriesModel* ChartModel::getSeries(int seriesIndex) const {
	if (seriesIndex >= 0 && seriesIndex < _series.size())
		return _series.at(seriesIndex);

	return NULL;
}

DataseriesModel* Ilwis::Ui::ChartModel::getSeriesByName(const QString name) const
{
	auto itr = std::find_if(_series.begin(), _series.end(), [](DataseriesModel* serie) { return serie->name() == "name"; });
	if (itr != _series.end()) {
		return *itr;
	}

	return NULL;
}

Ilwis::ITable ChartModel::table() const {
	return _table;
}

bool Ilwis::Ui::ChartModel::updateSeries() const
{
    return true;
}

bool Ilwis::Ui::ChartModel::isValidSeries(const QString columnName) const
{
	return _table->columndefinitionRef(columnName).isValid();
}

void ChartModel::updateDataSeries(int xaxis, int yaxis, int zaxis) {
    deleteSerie(xaxis, yaxis, zaxis);
    addDataSeries(xaxis, yaxis, zaxis);
    emit updateSeriesChanged();
}

void ChartModel::tickCountX(int tc) {
    if (tc > 0) {
        _tickCountX = tc;
        emit tickCountXChanged();
    }
}
int ChartModel::tickCountX() const {
    return _tickCountX;
}

void ChartModel::tickCountY(int tc) {
    if (tc > 0) {
        _tickCountY = tc;
        emit tickCountYChanged();
    }
}
int ChartModel::tickCountY() const {
    return _tickCountY;
}


quint32 Ilwis::Ui::ChartModel::addDataSeries(quint32 xaxis, quint32 yaxis, quint32 zaxis) {

    auto IntegerTicks = [](double res, double dist, int& tcount, double& minx, double& maxx)->void {
        tcount = (dist + 1) / res;
        if (tcount > 15) {
            tcount = 5;
            maxx = minx + tcount * std::ceil(dist / tcount);
        }
    };
	auto newseries = new DataseriesModel(this, xaxis, yaxis, zaxis);
	if (!newseries->setData())
		return _series.size();

	_series.push_back(newseries);

	if (isNumericalUndef(_minx) || isNumericalUndef(_maxx)) {
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
    double res = newseries->resolutionX();
    double dist = std::abs(_minx - _maxx);

    if (std::floor(res) == res) {
        IntegerTicks(res, dist, _tickCountX, _minx, _maxx);
    }
    res = newseries->resolutionY();
    dist = std::abs(_miny - _maxy);
    if (std::floor(res) == res) {
        IntegerTicks(res, dist, _tickCountY, _miny, _maxy);
    }


	emit xAxisChanged();
	emit yAxisChanged();
	emit chartModelChanged();

	return _series.size();
}


