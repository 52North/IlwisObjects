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

#include "kernel.h"  
#include "ilwisdata.h" 
#include "datadefinition.h"   
#include "columndefinition.h"         
#include "table.h"
#include "models/tablemodel.h"      
#include "chartoperationeditor.h"          
#include "symboltable.h"
#include "operationmetadata.h"   
#include "commandhandler.h"
#include "operationExpression.h"        
#include "operation.h"
#include "chartmodel.h"  
#include "dataseriesmodel.h"
#include "modelregistry.h"
#include "coveragedisplay\layermanager.h"
#include "coveragedisplay\layermodel.h" 
#include "histogramselectioncc.h"               

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("histogramselectioncc", HistogramSelectionCC)

HistogramSelectionCC::HistogramSelectionCC() : ChartOperationEditor("histogramselectioncc", TR("Histogram Selection"), QUrl("HistogramSelectionCC.qml"), TR("Interactive setting selections in the histogram and seeing results in the corresponding raster"))
{
}

bool HistogramSelectionCC::canUse(ChartModel *model, const QVariantMap &parameter) const  
{
	if (!parameter.empty())
		if (parameter["dataseries"].toBool()) {
			if (parameter.contains("specialtype")) {
				QString attr = parameter["attribute"].toString();
				bool ok = attr == "histogram_red" || attr == "histogram_green" || attr == "histogram_blue"; 
				const_cast<HistogramSelectionCC *>(this)->_band = attr;
				return parameter["specialtype"] == "histogram" && ok;     
			}
		}
	return false;
}

void HistogramSelectionCC::updateEditor(const QVariantMap& parameters) { 
	if (parameters["editor"] == "histogramselection") {
		initBands();
		QStringList parts = parameters["value"].toString().split("|");
		if (parts.size() == 3) {
			_cceditorRed->currentMinValue(parts[0].toDouble());
			_cceditorGreen->currentMinValue(parts[1].toDouble());
			_cceditorBlue->currentMinValue(parts[2].toDouble());   
		}

		_cceditorRed->currentValueChanged();
		_cceditorGreen->currentValueChanged(); 
		_cceditorBlue->currentValueChanged();  
	}
}

double HistogramSelectionCC::currentMinValue() const {   
	return _minValue;
}

void HistogramSelectionCC::currentMinValue(double v) {          
	initBands();
	_minValue = v;
	if (!_useRange) {
		_maxValue = v;
	}
}

double HistogramSelectionCC::currentMaxValue() const {  
	return _maxValue;
}

void HistogramSelectionCC::currentMaxValue(double v) { 
	initBands();
	_maxValue = v;
	if (!_useRange) {
		_minValue = v;
	}
}

bool HistogramSelectionCC::useRange() const {
	return _useRange;
}
void HistogramSelectionCC::useRange(bool yesno) { 
	_minValue = 9999999.0;
	_maxValue = 9999999.0;
	_useRange = yesno;
}

QString HistogramSelectionCC::getValues() {
	QString values;
	if (_useRange) {
		values = _cceditorRed ? (QString::number(_cceditorRed->currentMinValue(), 'f')) : "9999999.0";
		values += _cceditorRed ? ("|" + QString::number(_cceditorRed->currentMaxValue(), 'f')) : "|9999999.0";
		values += _cceditorGreen ? ("|" + QString::number(_cceditorGreen->currentMinValue(), 'f')) : "|9999999.0";
		values += _cceditorGreen ? ("|" + QString::number(_cceditorGreen->currentMaxValue(), 'f')) : "|9999999.0";
		values += _cceditorBlue ? (" | " + QString::number(_cceditorBlue->currentMinValue(), 'f')) : "|9999999.0";
		values += _cceditorBlue ? ("|" + QString::number(_cceditorBlue->currentMaxValue(), 'f')) : "|9999999.0";
	}
	else {
		values = _cceditorRed ? (QString::number(_cceditorRed->currentMinValue(), 'f')) : "9999999.0";
		values += _cceditorGreen ? ("|" + QString::number(_cceditorGreen->currentMinValue(), 'f')) : "|9999999.0";
		values += _cceditorBlue ? (" | " + QString::number(_cceditorBlue->currentMinValue(), 'f')) : "|9999999.0";
	}

	return values;
		
}

void HistogramSelectionCC::initBands() {
	if (_cceditorBlue == 0 || _cceditorGreen == 0 || _cceditorRed == 0) {
		DataseriesModel *series = chartModel()->getSeriesByName("histogram_red");
		_cceditorRed = static_cast<HistogramSelectionCC *>(series->operation("histogramselectioncc"));
		series = chartModel()->getSeriesByName("histogram_green");
		_cceditorGreen = static_cast<HistogramSelectionCC *>(series->operation("histogramselectioncc"));
		series = chartModel()->getSeriesByName("histogram_blue");
		_cceditorBlue = static_cast<HistogramSelectionCC *>(series->operation("histogramselectioncc")); 
	}
}
double HistogramSelectionCC::bandMinValue(const QString& bandName)  {
	initBands();
	if (bandName == "histogram_red" && _cceditorRed) {
		return _cceditorRed->currentMinValue();
	}
	if (bandName == "histogram_green" && _cceditorGreen) { 
		return _cceditorGreen->currentMinValue();
	}
	if (bandName == "histogram_blue" && _cceditorBlue) {
		return _cceditorBlue->currentMinValue();
	}
	return -9999999.0;
}

double HistogramSelectionCC::bandMaxValue(const QString& bandName) {
	initBands();
	if (bandName == "histogram_red" && _cceditorRed) {
		return _cceditorRed->currentMaxValue();
	}
	if (bandName == "histogram_green" && _cceditorGreen) {
		return _cceditorGreen->currentMaxValue();
	}
	if (bandName == "histogram_blue" && _cceditorBlue) {
		return _cceditorBlue->currentMaxValue();
	}
	return -9999999.0;
}

QObject *HistogramSelectionCC::view() {
	return chartModel()->view();
}

QString HistogramSelectionCC::band() const {
	return _band;
}

void HistogramSelectionCC::markersConfirmed() {
	auto modelPair = modelregistry()->getModel(chartModel()->linkedModelId());
	if (modelPair.first == "rastercoverage") {
		LayerModel *rasterLayer = dynamic_cast<LayerModel *>(modelPair.second);
		rasterLayer->layerManager()->needUpdate(true); // get prepare(ptGEOMETRY) be called at the next redraw
		rasterLayer->redraw();
	}

}

void HistogramSelectionCC::execute(const QVariantMap &parameters)
{
	try {

	}
	catch (const ErrorObject&) {

	}
}


