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
	CoverageLayerModel *cm = static_cast<CoverageLayerModel *>(vpmodel()->layer());
	quint64 objectid = cm->coverage()->id();
	bool ok;
	Resource resource = mastercatalog()->id2Resource(objectid);
	IlwisObjectModel *ioModel = new IlwisObjectModel(resource, parent);
	if (ioModel->isValid()) {
		return ioModel;
	}
	return 0;

}



