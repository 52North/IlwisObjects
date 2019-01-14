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
#include "modeller/modellerfactory.h"       
#include "commandhandler.h" 
#include "operationmetadata.h"   
#include "geos/geom/Coordinate.h"         
#include "location.h"
#include "ilwiscoordinate.h" 
#include "box.h" 
#include "supervisedclassification.h"      

using namespace Ilwis;
using namespace Ui;      


SupervisedClassification::SupervisedClassification() : Ilwis::AnalysisPattern()
{
    Init(); 
}

void SupervisedClassification::Init()     
{
}

bool SupervisedClassification::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
  return true;
}

IWorkflow SupervisedClassification::workflow(const IOOptions &opt)
{
    return IWorkflow();
}

void SupervisedClassification::store(QDataStream &stream)
{
    stream << type();
    AnalysisPattern::store(stream);
	QString raster = _multiSpectralRaster.isValid() ? _multiSpectralRaster->resource().url().toString() : "";
	stream << raster;
}

void SupervisedClassification::loadMetadata(QDataStream &stream)
{
    AnalysisPattern::loadMetadata(stream);
	QString sraster;
	stream >> sraster;
	if (sraster != "") {
		if (!_multiSpectralRaster.prepare(sraster)) {
			kernel()->issues()->log(TR("Couldnt load model. Raster is not available: ") + sraster);
		}
	}
}

QString SupervisedClassification::type() const
{
    return "supervisedclassification";
}

SupervisedClassification::SupervisedClassification(const QString &name, const QString &description) : AnalysisPattern(name, description)
{
    Init(); 
}

SupervisedClassification::AnalysisPattern *SupervisedClassification::create(const QString &name, const QString &description, const IOOptions &options)
{
    SupervisedClassification *sc =  new SupervisedClassification(name, description);
	if (options.contains("multispectralraster")) {
		QString sraster = options["multispectralraster"].toString();
		IRasterCoverage raster;
		if (raster.prepare(sraster)) {
			QVariant data;
			data.setValue<IRasterCoverage>(raster);
			sc->addData(SCRASTERKEY, data);
		}
	}
    return sc;
}

void SupervisedClassification::addData(const QString& key, const QVariant& var) {

	if (key == SCRASTERKEY) {
		IRasterCoverage raster = var.value<IRasterCoverage>();
		if (raster.isValid()) {
			_multiSpectralRaster = raster;
		}
	}
}

QVariant SupervisedClassification::data(const QString& key) const {
	QVariant data;
	data.setValue<IRasterCoverage>(_multiSpectralRaster);
	return data;
}


