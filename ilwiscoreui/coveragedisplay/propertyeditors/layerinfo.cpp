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

#include "coverage.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "mastercatalogmodel.h"
#include "layerinfo.h"

using namespace Ilwis;
using namespace Ui;

LayerInfo::LayerInfo()
{

}

LayerInfo::LayerInfo(VisualAttribute *p) :
	VisualPropertyEditor(p, "LayerInfo", TR("Layer Info"), QUrl("LayerInfo.qml"))
{

}

bool LayerInfo::canUse(const IIlwisObject& obj, const QString& name) const
{

	if (!obj.isValid())
		return false;
	if (!hasType(obj->ilwisType(), itCOVERAGE))
		return false;
	return name == VisualAttribute::LAYER_ONLY;

}

VisualPropertyEditor *LayerInfo::create(VisualAttribute *p)
{
	return new LayerInfo(p);
}

IlwisObjectModel *LayerInfo::object(QObject *parent) {
	LayerModel *layer = vpmodel()->layer();

	quint64 objectid = layer->vproperty("id").toULongLong();
	bool ok;
	Resource resource = mastercatalog()->id2Resource(objectid);
	IlwisObjectModel *ioModel = new IlwisObjectModel(resource, parent);
	if (ioModel->isValid()) {
		return ioModel;
	}

	return 0;

}



