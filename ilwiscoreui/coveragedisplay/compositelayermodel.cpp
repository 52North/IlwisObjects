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
