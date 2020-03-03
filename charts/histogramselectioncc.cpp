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
			_cceditorRed->currentValue(parts[0].toDouble());
			_cceditorGreen->currentValue(parts[1].toDouble());
			_cceditorBlue->currentValue(parts[2].toDouble()); 
		}

		_cceditorRed->currentValueChanged();
		_cceditorGreen->currentValueChanged();
		_cceditorBlue->currentValueChanged();
	}
}

double HistogramSelectionCC::currentValue() const {
	return _value;
}

void HistogramSelectionCC::currentValue(double v) {
	_value = v;
}

QString HistogramSelectionCC::getValues(double localX) {
	_value = localX;
	QString values;
	values = _cceditorRed ? (QString::number(_cceditorRed->currentValue(), 'f')) : "9999999.0";

	values += _cceditorGreen ? ("|"+ QString::number(_cceditorGreen->currentValue(), 'f')) : "|9999999.0";  

	values += _cceditorBlue ? (" | " + QString::number(_cceditorBlue->currentValue(), 'f')) : "|9999999.0";   

	return values;
		
}

double HistogramSelectionCC::currentX() const { 
		return _currentX;
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
double HistogramSelectionCC::bandX(const QString& bandName)  {
	initBands();
	if (bandName == "histogram_red" && _cceditorRed) {
		return _cceditorRed->currentX();
	}
	if (bandName == "histogram_green" && _cceditorGreen) { 
		return _cceditorGreen->currentX();
	}
	if (bandName == "histogram_blue" && _cceditorBlue) {
		return _cceditorBlue->currentX();
	}
	return -100000;
}

void HistogramSelectionCC::setXs(int redX, int greenX, int blueX) {
	initBands();
	_cceditorRed->_currentX = redX;
	_cceditorBlue->_currentX = blueX;
	_cceditorGreen->_currentX = greenX;

	_cceditorRed->currentXChanged();
	_cceditorGreen->currentXChanged();
	_cceditorBlue->currentXChanged();
}

double HistogramSelectionCC::bandValue(const QString& bandName) {
	initBands();
	if (bandName == "histogram_red" && _cceditorRed) {
		return _cceditorRed->currentValue();
	}
	if (bandName == "histogram_green" && _cceditorGreen) {
		return _cceditorGreen->currentValue();
	}
	if (bandName == "histogram_blue" && _cceditorBlue) {
		return _cceditorBlue->currentValue();
	}
	return 9999999.0;
}

void HistogramSelectionCC::currentX(double x) {

	initBands();
	if (_band == "histogram_red") {
		_cceditorRed->_currentX = x;
	}else if (_band == "histogram_green") {
		_cceditorGreen->_currentX = x;
	}
	else {
		_cceditorBlue->_currentX = x;
	}
	emit currentXChanged();
}

QObject *HistogramSelectionCC::view() {
	return chartModel()->view();
}

QString HistogramSelectionCC::band() const {
	return _band;
}

void HistogramSelectionCC::execute(const QVariantMap &parameters)
{
	try {

	}
	catch (const ErrorObject&) {

	}
}


