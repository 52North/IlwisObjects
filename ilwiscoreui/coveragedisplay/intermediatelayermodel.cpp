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
IntermediateLayerModel::IntermediateLayerModel(TreeNode *parent)
	: LayerModel(parent)
{
}

IntermediateLayerModel::IntermediateLayerModel(LayerManager *manager, QObject *parent, const QString& name, const QString& desc, const IOOptions& opt) : LayerModel(manager, parent, name, desc, opt) {
	//_layerType = itCOMPOSITELAYER;
	_isValid = true;
	_icon = "layers_s_green.png";
	readonly(true);
}

IntermediateLayerModel::~IntermediateLayerModel()
{
}

LayerModel *IntermediateLayerModel::create(LayerManager *manager, LayerModel *layer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new IntermediateLayerModel(manager, layer, name, desc, options);
}
