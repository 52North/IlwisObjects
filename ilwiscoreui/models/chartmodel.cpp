#include <random>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "modelregistry.h"
#include "tablemodel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "chartoperationfactory.h"
#include "chartoperationeditor.h"
#include "chartmodel.h"
#include "dataseriesmodel.h"
#include "colorrange.h"
#include "mathhelper.h"
#include "raster.h"
#include "ilwiscontext.h"

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

    _datatable.prepare();
}

ChartModel::~ChartModel()
{
    modelregistry()->unRegisterModel(modelId());
}

quint32 ChartModel::createChart(const QString& name, const ITable & tbl, const QString & cType, const QString& xaxis, const QString& yaxis, const QString& zaxis)
{
    _name = name;
    chartType(cType);
     
	_series = QList<DataseriesModel *>(); 
    QColor clr = newColor();
	insertDataSeries(tbl, 0, xaxis, yaxis, zaxis, clr);		// add the first dataseries

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

QString ChartModel::mainPanelUrl()
{
    return "../../extensions/ui/Charts/qml/ChartsPanel.qml";
}

QString ChartModel::minimalPanelUrl() {
    auto baseLoc = context()->ilwisFolder();
    QString loc =  baseLoc.absoluteFilePath() + "/extensions/ui/Charts/qml/MinimalChartPane.qml";
    return QUrl::fromLocalFile(loc).toString();
}

QString ChartModel::dataTableUrl()
{
    if (_datatable.isValid()) {
        return _datatable->resource().url().toString();
    }
    return QString();
}

void ChartModel::chartType(const QString & tp)
{
    _chartType = tp;
    emit chartTypeChanged();
}

QString ChartModel::chartType() const
{
    return _chartType;
}

void ChartModel::fillOperations() {
    auto *factory = Ilwis::kernel()->factory<ChartOperationFactory>("ilwis::chartoperationfactory");
    if (factory) {
        QVariantMap parameters = { { "chart", true } };
        _operations = factory->selectedOperations(this, parameters);
        for (auto iter = _operations.begin(); iter != _operations.end(); ++iter)
            (*iter)->setParent(this);
    }
}

QQmlListProperty<ChartOperationEditor> ChartModel::operations()
{
    if (_operations.isEmpty())
        fillOperations();

    return QQmlListProperty<ChartOperationEditor>(this, _operations);
}

ChartOperationEditor * ChartModel::operation(quint32 index)
{
    if (_operations.isEmpty())
        fillOperations();

    if (index < _operations.size())
        return _operations[index];

    return nullptr;
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

quint32 ChartModel::deleteSerie(const QString& ycolumn, const QString& zcolumn)  {
    for (int i = 0; i < _series.size(); ++i) {
        auto *series = _series[i];
        if (series->yColumn() == ycolumn && series->zColumn() == zcolumn) {
            _series.removeAt(i);
            emit updateSeriesChanged();
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

bool ChartModel::addDataTable(const QString & objid, const QString& xcolumn, const QString& ycolumn, const QString& color) {
    bool ok;
    quint64 id = objid.toULongLong(&ok);
    if (!ok) {
        kernel()->issues()->log(TR("Not a valid identifier for a table. Table could not be opened: ") + objid);
        return false;
    }
    IIlwisObject obj;
    if (!obj.prepare(id)) {
        kernel()->issues()->log(TR("Table could not be opened: ") + objid);
        return false;
    }
    ITable tbl;
    if (hasType(obj->ilwisType(), itTABLE)) {
        tbl = obj.as<Table>();
    }
    else if (hasType(obj->ilwisType(), itRASTER)) {
        IRasterCoverage raster = obj.as<RasterCoverage>();
        tbl = raster->histogramAsTable();
    }

    if (tbl->columnCount() < 2) {
        kernel()->issues()->log(TR("Not enough columns for adding a datasereis. At least 2 is needed: ") + objid);
        return false;
    }
    int xcIndex = 0, ycIndex = 1;
    if (xcolumn != sUNDEF) {
        xcIndex = tbl->columnIndex(xcolumn);
        if (xcIndex == iUNDEF) {
            kernel()->issues()->log(TR("Column does not exist: ") + xcolumn);
            return false;
        }
    }
    if (ycolumn != sUNDEF) {
        ycIndex = tbl->columnIndex(ycolumn);
        if (ycIndex == iUNDEF) {
            kernel()->issues()->log(TR("Column does not exist: ") + ycolumn);
            return false;
        }
    }
    if (_series.size() > 0) {
        if (axisCompatible(tbl->columndefinition(xcIndex).datadef(), ChartModel::Axis::AXAXIS)) {
            if (ycolumn == sUNDEF) {
                for (int c = 1; c < tbl->columnCount(); ++c) {
                    if (axisCompatible(tbl->columndefinition(c).datadef(), ChartModel::Axis::AYAXIS)) {
                        insertDataSeries(tbl, _series.size(), tbl->columndefinition(xcIndex).name(), tbl->columndefinition(c).name(), sUNDEF, color);
                    }
                }
            }
            else {
                insertDataSeries(tbl, _series.size(), tbl->columndefinition(xcIndex).name(), tbl->columndefinition(ycIndex).name(), sUNDEF, color);
            }
            emit updateSeriesChanged();
        }
    }
    else {
        createChart("histogram", tbl, "line", xcolumn, ycolumn, sUNDEF);
        emit updateSeriesChanged();
    }
    return true;
}
void ChartModel::assignParent(QObject * parent)
{
    setParent(parent);
}
bool ChartModel::addDataTable(const QString & objid)
{
    return addDataTable(objid, sUNDEF, sUNDEF, sUNDEF);
}

DataseriesModel* ChartModel::getSeriesByName(const QString name) const
{
	auto itr = std::find_if(_series.begin(), _series.end(), [name](DataseriesModel* serie) { return serie->name() == name; });
	if (itr != _series.end()) {
		return *itr;
	}

	return NULL;
}

bool ChartModel::updateSeries() const
{
    return true;
}

bool ChartModel::isValidSeries(const ITable& inputTable, const QString columnName) const
{
	return inputTable->columndefinition(columnName).isValid();
}

void ChartModel::updateDataSeries(const ITable& inputTable, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn) {
    auto *serie = getSeries(xcolumn, ycolumn, zcolumn);
    bool contPin = ycolumn == "contineous_pin";
    QColor currentColor = serie ? serie->color() : newColor(); 
    quint32 index = deleteSerie(ycolumn, zcolumn);
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

double ChartModel::minx() const {
    return _minx;
}

double ChartModel::maxx() const {
    return _maxx;
}

double ChartModel::miny() const {
    return _miny;
}

double ChartModel::maxy() const {
    return _maxy;
}

void ChartModel::setMinX(double val) {
    _minx = val; emit xAxisChanged();
}

void ChartModel::setMaxX(double val) {
    _maxx = val; emit xAxisChanged();
}

void ChartModel::setMinY(double val) {
    _miny = val; emit yAxisChanged();
}

void ChartModel::setMaxY(double val) {
    _maxy = val; emit yAxisChanged();
}

bool ChartModel::fixedYAxis() const {
    return _fixedY;
}

void ChartModel::setFixedYAxis(bool fixed) {
    _fixedY = fixed; emit yAxisChanged();
}

bool ChartModel::niceNumbersY() const {
    return _niceNumbersY;
}
void ChartModel::setNiceNumbersY(bool nice) {
    _niceNumbersY = nice; emit yAxisChanged();
}

bool ChartModel::legendVisible() const
{
    return _legendVisible;
}

void ChartModel::setLegendVisible(bool show) {
    _legendVisible = show;
    emit legendChanged();
    emit chartModelChanged();
}

QString ChartModel::legendAlignment() const {
    return _alignment;
}

void ChartModel::setLegendAlignment(const QString& align) {
    _alignment = align;
    emit legendChanged();
    emit chartModelChanged();
}

bool ChartModel::xAxisVisble()
{
    if (_chartType == "pie")
        return false;

    return true;
}

bool ChartModel::yAxisVisble()
{
    if (_chartType == "pie")
        return false;

    return true;
}

QColor ChartModel::newColor() const
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

    if (_series.size() == 0) {
        _xaxisType = ChartModel::AxisType(newseries->xAxisType());
    }

    newseries->setType(_chartType);

    _series.insert(index, newseries);

    initializeDataSeries(newseries);

    emit xAxisChanged();
    emit yAxisChanged();
    emit chartModelChanged();

    return _series.size();

}

void ChartModel::initializeDataSeries(DataseriesModel *newseries) {
    auto IntegerTicks = [](double res, double dist, int& tcount, double& minx, double& maxx)->void {
        if (res == 0)
            tcount = 5;
        else {
            tcount = (dist + 1) / res;
            if (tcount > 15) {
                tcount = 5;
                maxx = minx + tcount * std::ceil(dist / tcount);
            }
        }
    };

    if (_xaxisType == AxisType::AT_VALUE) {
        if (isNumericalUndef(_minx) || isNumericalUndef(_maxx)) {
            _minx = newseries->minx();
            _maxx = newseries->maxx();
        }
        else {
            if (!_fixedX) _minx = std::min(_minx, newseries->minx());
            if (!_fixedX) _maxx = std::max(_maxx, newseries->maxx());
        }
        double res = newseries->resolutionX();
        double dist = std::abs(_minx - _maxx);

        if (std::floor(res) == res) {
            IntegerTicks(res, dist, _tickCountX, _minx, _maxx);
        }
    }
    else if (_xaxisType == AxisType::AT_CATEGORIES) {
        auto dd = newseries->datadefinition(Axis::AXAXIS);
        auto totalRange = dd.domain()->range();
        _tickCountX = totalRange->count();
    }

    if (newseries->yAxisType() == static_cast<quint16>(ChartModel::AxisType::AT_VALUE) ) {
        if (isNumericalUndef(_miny) || isNumericalUndef(_maxy)) {
            _miny = newseries->miny();
            _maxy = newseries->maxy();
        }
        else {
            if (!_fixedY) _miny = std::min(_miny, newseries->miny());
            if (!_fixedY) _maxy = std::max(_maxy, newseries->maxy());
        }
        double res = newseries->resolutionY();
        double dist = std::abs(_miny - _maxy);
        if (std::floor(res) == res) {
            IntegerTicks(res, dist, _tickCountY, _miny, _maxy);
        }
    }
    else if (newseries->yAxisType() == static_cast<quint16>(AxisType::AT_CATEGORIES)) {
        auto dd = newseries->datadefinition(Axis::AYAXIS);
        auto totalRange = dd.domain()->range();
        _tickCountY = totalRange->count();
    }
}

QString formatAxis(double res, const QString& result) {
    if (res == 0)
        return "%.6f";
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

quint16 ChartModel::xaxisType() const
{
    return (quint16)_xaxisType;
}

bool ChartModel::axisCompatible(const DataDefinition& inputDef, Axis axis, bool basicCheck)
{
    if (basicCheck && _series.size() > 0) {
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

QColor ChartModel::seriesColor(int seriesIndex) {
    if (seriesIndex < _series.size())
        return _series[seriesIndex]->color();
    return QColor();
}

QColor ChartModel::seriesColorItem(int seriesIndex, double v) {
 //TODO
    return QColor("red");
}


