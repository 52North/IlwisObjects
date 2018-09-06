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
#include "compositelayermodel.h"

using namespace Ilwis;
using namespace Ui;

CompositeLayerModel::CompositeLayerModel() {

}
CompositeLayerModel::CompositeLayerModel(LayerModel *parent)
	: LayerModel()
{
}

CompositeLayerModel::CompositeLayerModel(LayerManager *manager, QStandardItem *parent, const QString& name, const QString& desc, const IOOptions& opt) : LayerModel(manager, parent, name, desc, opt) {
	//_layerType = itCOMPOSITELAYER;
	_isValid = true;
	_icon = "layers_s_green.png";
	setEditable(true);
}

CompositeLayerModel::~CompositeLayerModel()
{
}

LayerModel *CompositeLayerModel::create(LayerManager *manager, QStandardItem *parentLayer, const QString &name, const QString &desc, const IOOptions& options)
{
	return new CompositeLayerModel(manager, parentLayer, name, desc, options);
}

void CompositeLayerModel::setActiveAttribute(int idx) {
}

int CompositeLayerModel::numberOfBuffers(const QString&) const {
    return 0;
}

bool CompositeLayerModel::renderReady() {
    return true; // TODO: return the correct value
}

void CompositeLayerModel::renderReady(bool yesno) {
    // TODO
}
