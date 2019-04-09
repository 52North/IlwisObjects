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
#include "tablemerger.h"
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

quint32 ChartModel::createChart(const QString& name, const ITable & tbl, const QString & cType, const QString& xaxis, const QString& yaxis, const QString& zaxis, const QVariantMap& extraParameters)
{
    _name = name;
    chartType(cType);
     
	_series = QList<DataseriesModel *>(); 
    QColor clr = newColor();
	auto extra = extraParameters;
	extra["color"] = extraParameters.contains("color") ? extraParameters["color"].value<QColor>() : clr;
	insertDataSeries(tbl, 0, xaxis, yaxis, zaxis, extra);		// add the first dataseries
	TableMerger merger;
	std::vector<QString> columnsToBeConsidered = { xaxis, yaxis };
	merger.simpleCopyColumns(tbl, _datatable, columnsToBeConsidered);
	merger.mergeTableData(tbl, _datatable, 0);

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
	auto baseLoc = context()->ilwisFolder();
	QString loc = baseLoc.absoluteFilePath() + "/extensions/ui/Charts/qml/ChartsPanel.qml";
	return QUrl::fromLocalFile(loc).toString();
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

bool ChartModel::addDataTable(const QString & objid, const QString& xcolumn, const QString& ycolumn, QVariantMap extraParams) {
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
	std::map<Raw, double> raw2value;
	if (_datatable->columnIndex(tbl->columndefinition(1).name()) == iUNDEF) {
		_datatable->addColumn(tbl->columndefinition(1));


		//merger.mergeTableData(tbl, _datatable, 0); 
		if (hasType(_datatable->columndefinition(0).datadef().domain()->ilwisType(), itITEMDOMAIN)) {
			if (_datatable->columndefinition(0).datadef().domain()->valueType() == itNUMERICITEM) {
				IIntervalDomain idomain = _datatable->columndefinition(0).datadef().domain().as<IntervalDomain>();
				for (auto item : idomain) {
					auto interval = item->as<Interval>();
					double mid = interval->range().center();
					raw2value[item->raw()] = mid;
				}
			}
		}
		std::map<double, std::vector<QVariant>> records;
		for (int i = 0; i < _datatable->recordCount(); ++i) {
			std::vector<QVariant> record = _datatable->record(i);
			if (raw2value.size() == 0)
				records[record[0].toDouble()] = record;
			else
				records[raw2value[record[0].toDouble()]] = record;
		}
		for (int i = 0; i < tbl->recordCount(); ++i) {
			std::vector<QVariant> srecord = tbl->record(i);
			auto iter = records.find(srecord[0].toDouble());
			if (iter != records.end()) {
				(*iter).second.back() = srecord[1];
			}
			else {
				std::vector<QVariant> record(_datatable->columnCount(), rUNDEF);
				record.back() = srecord[1];
				record[0] = srecord[0].toDouble();
				records[srecord[0].toDouble()] = record;
			}
		}
		_datatable->recordCount(0);
		for (auto item : records) {
			_datatable->newRecord() = item.second;
		}
		if ( _dataTableModel)
			_dataTableModel->setNewTable(_datatable);
	}

	if (tbl->name().indexOf("splib_") == 0) {
		extraParams["chartType"] = "line";
	}
    if (_series.size() > 0) {
        if (axisCompatible(tbl->columndefinition(xcIndex).datadef(), ChartModel::Axis::AXAXIS)) {
            if (ycolumn == sUNDEF) {
                for (int c = 1; c < tbl->columnCount(); ++c) {
                    if (axisCompatible(tbl->columndefinition(c).datadef(), ChartModel::Axis::AYAXIS)) {
                        insertDataSeries(tbl, _series.size(), tbl->columndefinition(xcIndex).name(), tbl->columndefinition(c).name(), sUNDEF, extraParams);
                    }
                }
            }
            else {
                insertDataSeries(tbl, _series.size(), tbl->columndefinition(xcIndex).name(), tbl->columndefinition(ycIndex).name(), sUNDEF, extraParams);
            }
            emit updateSeriesChanged();
			emit dataTableChanged();
        }
    }
    else {
		if (extraParams.size() > 0) {
			QString name = extraParams["name"].toString();
			QString type = extraParams["chartType"].toString();
			createChart(name, tbl, type, xcolumn, ycolumn, sUNDEF, extraParams);
			emit updateSeriesChanged();
			emit dataTableChanged();
		}
    }
    return true;
}

bool ChartModel::updateDataTable() const {
	return true;
}

void ChartModel::clearChart() {
	_series = QList<DataseriesModel *>();
	_minx = _maxx = _miny = _maxy = rUNDEF;
	_fixedY = false;
	_fixedX = false;
	_niceNumbersY = false;
	_legendVisible = true;
	_alignment = "top";

	_chartType = sUNDEF;
	_tickCountX = 5;
	_tickCountY = 5;
	_name = sUNDEF;
	if (_datatable.isValid()) {
		_datatable.prepare();
	}
}
void ChartModel::assignParent(QObject * parent)
{
    setParent(parent);
}
bool ChartModel::addDataTable(const QString & objid)
{
	if (addDataTable(objid, sUNDEF, sUNDEF, QVariantMap()))
		return true;
	kernel()->issues()->log(TR("Axis do not have a compatible domain with the existing chart"), IssueObject::itWarning);
	return false;
}

void ChartModel::changeDataSeriesName(const QString& oldName, const QString& newName) {
	auto * serie = getSeriesByName(oldName);
	if (serie) {
		serie->name(newName);
	}
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
    QColor currentColor = serie ? serie->color() : newColor(); 
    quint32 index = deleteSerie(ycolumn, zcolumn);
	QVariantMap extra;
	extra["color"] = currentColor;
    insertDataSeries(inputTable, index, xcolumn, ycolumn, zcolumn, extra);
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
}

QString ChartModel::legendAlignment() const {
    return _alignment;
}

void ChartModel::setLegendAlignment(const QString& align) {
    _alignment = align;
    emit legendChanged();
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

quint32 ChartModel::insertDataSeries(const ITable& inputTable, quint32 index, const QString& xcolumn, const QString& ycolumn, const QString& zcolumn, const QVariantMap& extra) {
    auto newseries = new DataseriesModel(this, xcolumn, ycolumn, zcolumn, extra);
    if (!newseries->setData(inputTable, extra))
        return _series.size();

    if (_series.size() == 0) {
        _xaxisType = ChartModel::AxisType(newseries->xAxisType());
    }

	if ( newseries->charttype() ==  sUNDEF)
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

        if (std::floor(res) == res) {
			 NumericRange rng = MathHelper::roundRange(_minx, _maxx);
			_tickCountX = 1 + rng.distance() / rng.resolution();
			_minx = rng.min();
			_maxx = rng.max();

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
				return  checkRanges(datadef.range<NumericRange>().get(), inputDef.range<NumericRange>().get());
			}
			else
				return true;
		}
		else { // special case of compatibility (in this case). IntervalDomain and NumericDomain
			if (hasType(itNUMERICDOMAIN | itITEMDOMAIN, datadef.domain()->ilwisType()) &&
				hasType(itNUMERICDOMAIN | itITEMDOMAIN, inputDef.domain()->ilwisType())) {
				if (hasType(itNUMERIC, datadef.domain()->valueType()) &&
					hasType(itNUMERIC, inputDef.domain()->valueType())) {
					SPNumericRange numrange1;
					NumericRange numrange2;
					if (datadef.domain()->ilwisType() == itNUMERICDOMAIN) {
						numrange1 = datadef.range<NumericRange>();
					}
					if (inputDef.domain()->ilwisType() == itNUMERICDOMAIN) {
						numrange1 = inputDef.range<NumericRange>();
					}
					if (datadef.domain()->valueType() == itNUMERICITEM) {
						numrange2 = datadef.range<IntervalRange>()->totalRange();
					}
					if (inputDef.domain()->valueType() == itNUMERICITEM) {
						numrange2 = inputDef.range<IntervalRange>()->totalRange();
					}
					return checkRanges(numrange1.get(), &numrange2);
				}
			}

		}
	}
    return false;
}

bool ChartModel::checkRanges(const NumericRange *range1, const NumericRange *range2) const
{
	double deltaMin = range1->min() - range2->min();
	double deltaMax = range1->max() - range2->max();
	double deltaMinMax = range1->max() - range1->min();
	if (deltaMin <= 0 && deltaMax >= 0)
		return true;
	//if (deltaMin > 0 && deltaMinMax * 0.25 < std::abs(deltaMin))
	if (deltaMin > 0 && std::abs(deltaMin) < deltaMinMax * 0.5)
		return true;
	if (deltaMax < 0 && std::abs(deltaMax) < deltaMinMax * 0.5)
		return true;
	return false;
}

QColor ChartModel::seriesColor(int seriesIndex) {
    if (seriesIndex < _series.size())
        return _series[seriesIndex]->color();
    return QColor();
}

QString ChartModel::dataTableId() const {
	if (_datatable.isValid()) {
		return QString::number(_datatable->id());
	}
	return QString();
}

TableModel *ChartModel::tableModel() const {
	return _dataTableModel;
}

void ChartModel::tableModel(TableModel *tbl)  {
	_dataTableModel = tbl;
}

QColor ChartModel::seriesColorItem(int seriesIndex, double v) {
 //TODO
    return QColor("red");
}


