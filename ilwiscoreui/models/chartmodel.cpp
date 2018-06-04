#include <random>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "modelregistry.h"
#include "tablemodel.h"
#include "chartmodel.h"
#include "dataseriesmodel.h"
#include "colorrange.h"
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

quint32 Ilwis::Ui::ChartModel::createChart(const QString& name, const ITable & tbl, const QString & cType, const QString& xaxis, const QString& yaxis, const QString& zaxis)
{
    _name = name;
    chartType(cType);
     
	_series = QList<DataseriesModel *>(); 
    QColor clr = newColor();
	addDataSeries(tbl, xaxis, yaxis, zaxis, clr);		// add the first dataseries

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
DataseriesModel* ChartModel::getSeries(const QString& xcolumn, const QString& ycolumn, const QString& zcolumn) const {
    for (auto *series : _series) {
        if (series->xColumn() == xcolumn && series->yColumn() == ycolumn && series->zColumn() == zcolumn)
            return series;
    }
    return 0;
}

quint32 ChartModel::deleteSerie(const QString& xcolumn, const QString& ycolumn, const QString& zcolumn)  {
    for (int i = 0; i < _series.size(); ++i) {
        auto *series = _series[i];
        if (series->xColumn() == xcolumn && series->yColumn() == ycolumn && series->zColumn() == zcolumn) {
            _series.removeAt(i);
            return i;
        }
    }
    return iUNDEF;
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

bool Ilwis::Ui::ChartModel::updateSeries() const
{
    return true;
}

bool Ilwis::Ui::ChartModel::isValidSeries(const ITable& inputTable, const QString columnName) const
{
	return inputTable->columndefinitionRef(columnName).isValid();
}

void ChartModel::updateDataSeries(const ITable& inputTable, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn) {
    auto *serie = getSeries(xcolumn, ycolumn, zcolumn);
    QColor currentColor = serie ? serie->color() : newColor(); 
    quint32 index = deleteSerie(xcolumn, ycolumn, zcolumn);
    insertDataSeries(inputTable, index, xcolumn, ycolumn, zcolumn, currentColor);
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

QColor Ilwis::Ui::ChartModel::newColor() const
{
     for (QColor clr : _graphColors) {
        bool found = false;
        for (auto *serie : _series) {
            if (serie->color() == clr) {
                found = true;
                break;
            }
        }
        if (!found)
            return clr;
    }
     std::random_device rd; 
     std::mt19937 gen(rd()); 
     std::uniform_int_distribution<> dis(0, _graphColors.size() - 1);
     return  dis(gen);
}

quint32 ChartModel::insertDataSeries(const ITable& inputTable, quint32 index, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn, const QColor& color) {
    auto newseries = new DataseriesModel(this, xcolumn, ycolumn, zcolumn, color);
    if (!newseries->setData(inputTable))
        return _series.size();
    _series.insert(index,newseries);

    initializeDataSeries(newseries);

    emit xAxisChanged();
    emit yAxisChanged();
    emit chartModelChanged();

    return _series.size();

}

void ChartModel::initializeDataSeries(DataseriesModel *newseries) {
    auto IntegerTicks = [](double res, double dist, int& tcount, double& minx, double& maxx)->void {
        tcount = (dist + 1) / res;
        if (tcount > 15) {
            tcount = 5;
            maxx = minx + tcount * std::ceil(dist / tcount);
        }
    };

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
}

QString formatAxis(double res, const QString& result) {
    if (res == 0)
        return "%f";
    if (res - (quint64)res > 0) {
        int n = std::abs(log10(res - (quint64)res));
        return QString("%.%1f").arg(n);
    }
    if (res - (quint64)res == 0 && result == "")
       return "%.0f" ;

    return "";
}
QString ChartModel::formatXAxis() const
{
    QString result = "";
    for (auto *serie : _series) {
        result = formatAxis(serie->resolutionX(), result);
    }
    return result;
}

QString ChartModel::formatYAxis() const
{
    QString result = "";
    for (auto *serie : _series) {
        result = formatAxis(serie->resolutionY(), result);
    }
    return result;
}
quint32 ChartModel::addDataSeries(const ITable& inputTable, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QColor& color) {


	auto newseries = new DataseriesModel(this, xaxis, yaxis, zaxis, color);
	if (!newseries->setData(inputTable))
		return _series.size();

	_series.push_back(newseries);

    initializeDataSeries(newseries);

	emit xAxisChanged();
	emit yAxisChanged();
	emit chartModelChanged();

	return _series.size();
}

QColor ChartModel::seriesColor(int seriesIndex) {
    if (seriesIndex < _series.size())
        return _series[seriesIndex]->color();
    return QColor();
}

QColor ChartModel::seriesColorItem(int seriesIndex, double v) {
 //TODO
    return QColor("red");
}


