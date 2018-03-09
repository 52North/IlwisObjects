#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "layermanager.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "intermediatelayermodel.h"

using namespace Ilwis;
using namespace Ui;

IntermediateLayerModel::IntermediateLayerModel() {

}
IntermediateLayerModel::IntermediateLayerModel(LayerModel *parent)
	: LayerModel()
{
}

IntermediateLayerModel::IntermediateLayerModel(LayerManager *manager, QStandardItem *parent, const QString& name, const QString& desc, const IOOptions& opt) : LayerModel(manager, parent, name, desc, opt) {
	//_layerType = itCOMPOSITELAYER;
	_isValid = true;
	_icon = "layers_s_green.png";
	setEditable(true);
}

IntermediateLayerModel::~IntermediateLayerModel()
{
}

LayerModel *IntermediateLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new IntermediateLayerModel(manager, parentLayer, name, desc, options);
}

void IntermediateLayerModel::setActiveAttribute(int idx) {
}

int IntermediateLayerModel::numberOfBuffers(const QString&) const {
    return 0;
}
