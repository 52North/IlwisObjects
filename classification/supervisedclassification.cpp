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
using namespace SClassification;

REGISTER_ANALYSISPATTERN(SupervisedClassification)

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


