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
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "modeller/applicationmodel.h"
#include "modeller/model.h"
#include "modeller/analysispattern.h"
#include "workflow/modelbuilder.h"
#include "supervisedclassification.h"
#include "supervisedclassificationmodel.h"


using namespace Ilwis::Ui;
using namespace Ilwis;
using namespace SClassification;

REGISTER_ANALYSISMODEL(Ilwis::SClassification::SupervisedClassificationmodel, "supervisedclassification")

SupervisedClassificationmodel::SupervisedClassificationmodel()
{
 
}

SupervisedClassificationmodel::SupervisedClassificationmodel(Ilwis::AnalysisPattern *p) : AnalysisModel(p){
    _panels["main"] = "Classification/qml/SupervisedClassificationPanel.qml";
    _panels["form"] = "Classification/qml/SupervisedClassificationForm.qml";
}

AnalysisModel *SupervisedClassificationmodel::create(Ilwis::AnalysisPattern *pattern)
{
    return new SupervisedClassificationmodel(pattern);
}

QVariantMap SupervisedClassificationmodel::execute(const QVariantMap parameters)
{
    QVariantMap output;
    if ( _analysis){
        _analysis->execute(parameters, output);
    }
    return output;
}

QString SupervisedClassificationmodel::multispectralraster()  {
	QString result;
	if (_analysis) {
		QVariant data = _analysis->data(SCRASTERKEY);
		if (data.isValid()) {
			IRasterCoverage raster = data.value<IRasterCoverage>();
			if (raster.isValid()) {
				result = raster->resource().url().toString();
			}
		}
	}
	return result;

}

void SupervisedClassificationmodel::multispectralraster(const QString& msr) {
	if (_analysis) {
		IRasterCoverage raster;
		if (raster.prepare(msr)) {
			QVariant data;
			data.setValue<IRasterCoverage>(raster);
			_analysis->addData(SCRASTERKEY, data);
			emit multispectralrasterChanged();
		}
	}
}

