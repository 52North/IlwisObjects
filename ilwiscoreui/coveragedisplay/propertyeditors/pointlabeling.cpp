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
#include "featurecoverage.h"
#include "ilwiscontext.h"
#include "pointlabeling.h"


using namespace Ilwis;
using namespace Ui;

PointLabeling::PointLabeling()
{

}

PointLabeling::PointLabeling(VisualAttribute *p) :
	VisualPropertyEditor(p, "pointlabellayereditor", TR("Point Labels"), QUrl("PointLabeling.qml"))
{

}

bool PointLabeling::canUse(const IIlwisObject& obj, const DataDefinition &def) const
{
	if (!obj.isValid())
		return false;
	if (!hasType(obj->ilwisType(), itFEATURE))
		return false;
	if (!def.isValid() || !def.domain().isValid())
		return false;
	if (!hasType(def.domain()->ilwisType(), itTEXTDOMAIN | itNUMERICDOMAIN | itITEMDOMAIN))
		return false;
	IFeatureCoverage features = obj.as<FeatureCoverage>();
	return  features->featureCount(itPOINT) > 0;
}

VisualPropertyEditor *PointLabeling::create(VisualAttribute *p)
{
	return new PointLabeling(p);
}

/*void PointLabeling::prepare(const Ilwis::IIlwisObject& bj, const DataDefinition &datadef) {
	auto *lm = vpmodel()->layer()->layerManager();
	QString path = context()->ilwisFolder().absoluteFilePath();
	QUrl url = QUrl::fromLocalFile(path);
	associatedUrl(url.toString() + "/qml/datapanel/visualization/propertyeditors/PostDrawerPointLabels.qml");
	lm->addPostDrawer(this);
}*/

int PointLabeling::labelPlacement() const {
	return _placement;
}

void PointLabeling::labelPlacement(int p) {
	_placement = (LabelPlacement)p;
}

