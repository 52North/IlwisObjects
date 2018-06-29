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

Ilwis::Ui::ChartModel::~ChartModel()
{
    modelregistry()->unRegisterModel(modelId());
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

QQmlListProperty<DataseriesModel> ChartModel::series()
{
	return QQmlListProperty<DataseriesModel>(this, _series);
}

DataseriesModel* ChartModel::getSeries(int seriesIndex) const {
	if (seriesIndex >= 0 && seriesIndex < _series.size())
		return _series.at(seriesIndex);

	return NULL;
}

bool Ilwis::Ui::ChartModel::addDataTable(const QString & objid)
{
    bool ok;
    quint64 id = objid.toULongLong(&ok);
    if (!ok) {
        kernel()->issues()->log(TR("Not a valid identifier for a table. Table could not be opened: ") + objid);
        return false;
    }
    ITable tbl;
    if (!tbl.prepare(id)) {
        kernel()->issues()->log(TR("Table could not be opened: ") + objid);
        return false;
    }
    if (tbl->columnCount() < 2) {
        kernel()->issues()->log(TR("Not enough column for adding a datasereis. At least 2 is needed: ") + objid);
        return false;
    }
    if (axisCompatible(tbl->columndefinition(0).datadef(), ChartModel::aXAXIS)) {
        for (int c = 1; c < tbl->columnCount(); ++c) {
            if (axisCompatible(tbl->columndefinition(c).datadef(), ChartModel::aYAXIS)) {
                addDataSeries(tbl, tbl->columndefinition(0).name(), tbl->columndefinition(c).name(), sUNDEF, sUNDEF);
            }
        }
            emit updateSeriesChanged();
    }
    return true;
}

DataseriesModel* Ilwis::Ui::ChartModel::getSeriesByName(const QString name) const
{
	auto itr = std::find_if(_series.begin(), _series.end(), [name](DataseriesModel* serie) { return serie->name() == name; });
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
	return inputTable->columndefinition(columnName).isValid();
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
bool Ilwis::Ui::ChartModel::axisCompatible(const DataDefinition& inputDef, Axis axis, bool basicCheck)
{
    if (basicCheck) {
        // in the basic check we only check if it fits the first dataseries (axis)
        DataseriesModel *serie = _series[0];
        DataDefinition datadef = serie->datadefinition(axis);
        if (inputDef.domain()->isCompatibleWith(datadef.domain().ptr())) {
            if (datadef.domain()->ilwisType() == itNUMERICDOMAIN) {
                double deltaMin = datadef.range<NumericRange>()->min() - inputDef.range<NumericRange>()->min();
                double deltaMax = datadef.range<NumericRange>()->max() - inputDef.range<NumericRange>()->max();
                double deltaMinMax = datadef.range<NumericRange>()->max() - datadef.range<NumericRange>()->min();
                if (deltaMin <= 0 && deltaMax >= 0)
                    return true;
                if (deltaMin > 0 && deltaMinMax * 0.25 < std::abs(deltaMin))
                    return true;
                if (deltaMax < 0 && std::abs(deltaMax) < deltaMinMax * 0.25)
                    return true;

            }else
                return true;
        }
    }
    return false;
}
quint32 ChartModel::addDataSeries(const ITable& inputTable, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QColor& color) {

	auto newseries = new DataseriesModel(this, xaxis, yaxis, zaxis, color);
	if (!newseries->setData(inputTable))
		return _series.size();

    newseries->setType(_chartType);
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


