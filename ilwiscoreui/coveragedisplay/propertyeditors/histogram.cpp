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
#include "representation.h"
#include "geometries.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "raster.h"
#include "coveragelayermodel.h"
#include "table.h"
#include "ilwiscontext.h"
#include "histogram.h"


using namespace Ilwis;
using namespace Ui;

Histogram::Histogram()
{

}

Histogram::Histogram(VisualAttribute *p) :
	VisualPropertyEditor(p, "histogrameditor", TR("Histogram"), QUrl("Histogram.qml"))
{

}

bool Histogram::canUse(const IIlwisObject &obj, const QString &name) const
{
	if (!obj.isValid())
		return false;
	if (name == VisualAttribute::LAYER_ONLY)
		return false;

	if (!hasType(obj->ilwisType(), itCOVERAGE))
		return false;
	Ilwis::ICoverage cov = obj.as<Coverage>();

	IDomain dom = vpmodel()->domain();
	bool useable = dom.isValid() && dom->ilwisType() == itNUMERICDOMAIN;
	return !vpmodel()->layer()->isSupportLayer() && useable;
}

bool Histogram::canUse(const IIlwisObject &, const DataDefinition &def) const
{
	if (def.isValid())
		return hasType(def.domain()->ilwisType(), itNUMERICDOMAIN);
	return false;
}

VisualPropertyEditor *Histogram::create(VisualAttribute *p)
{
	return new Histogram(p);
}

QString Histogram::tableUrlPrivate()
{
	if (_histogramData.isValid()) {
		return  _histogramData->resource().url().toString();
	}
	return QString();
}

void Histogram::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef) {
	auto *layer = vpmodel()->layer();
	auto *clayer = layer->as<CoverageLayerModel>();
	IRasterCoverage raster = clayer->coverage().as<RasterCoverage>();
	if (raster.isValid()) {
		_histogramData = raster->histogramAsTable(PIXELVALUE);
	}
}



