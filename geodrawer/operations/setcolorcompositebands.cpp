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
#include "geometries.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "raster.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "rasterlayermodel.h"
#include "colorcompositemodel.h"
#include "coveragedisplay/layermanager.h"
#include "operationhelper.h"
#include "setcolorcompositebands.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetColorCompositeBands)

SetColorCompositeBands::SetColorCompositeBands()
{

}

SetColorCompositeBands::~SetColorCompositeBands()
{

}

SetColorCompositeBands::SetColorCompositeBands(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetColorCompositeBands::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	QString ids;
	for (auto rasterid : _rasters) {
		if ( ids != "")
			ids += "|";
		ids += QString::number(rasterid);
	}
	_cclayer->vproperty("colorcompositerasters", ids);
	return true;
}

Ilwis::OperationImplementation *SetColorCompositeBands::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new SetColorCompositeBands(metaid, expr);
}

Ilwis::OperationImplementation::State SetColorCompositeBands::prepare(ExecutionContext *ctx, const SymbolTable &)
{
	if (!getViewId(_expression.input<QString>(0))) {
		return sPREPAREFAILED;
	}

	auto *layer = layerManager()->findLayer(_expression.input<QString>(1).toInt());
	_cclayer = dynamic_cast<ColorCompositeLayerModel *>(layer);
	if (!_cclayer) {
		kernel()->issues()->log(TR("Layer used for colorcomposite changes is not valid"));
		return sPREPAREFAILED;
	}
	IGeoReference grf = _cclayer->georeference();
	if (!grf.isValid()) {
		kernel()->issues()->log(TR("Layer used for colorcomposite changes is not valid"));
		return sPREPAREFAILED;
	}
	std::vector<IRasterCoverage> rasters(3);
	OperationHelper::check([&]()->bool { return rasters[0].prepare(_expression.input<QString>(2), itRASTER); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(2), "" });
	OperationHelper::check([&]()->bool { return rasters[1].prepare(_expression.input<QString>(3), itRASTER); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(3), "" });
	OperationHelper::check([&]()->bool { return rasters[2].prepare(_expression.input<QString>(4), itRASTER); },
		{ ERR_COULD_NOT_LOAD_2,_expression.input<QString>(4), "" });

	for (IRasterCoverage& raster : rasters) {
		if (!raster->georeference()->isCompatible(grf)) {
			kernel()->issues()->log(TR("Color composite raster band has incompatible georeference"));
			return sPREPAREFAILED;
		}
		_rasters.push_back(raster->id());
	}

	return sPREPARED;
}

quint64 SetColorCompositeBands::createMetadata()
{
	OperationResource operation({ "ilwis://operations/setcolorcompositebands" });
	operation.setSyntax("setcolorcompositebands(viewid, layerindex,rasterRed, rasterGreen, rasterBlue)");
	operation.setDescription(TR("Sets the opacity ( alpha channel) of the layer indicated by the index"));
	operation.setInParameterCount({ 5 });
	operation.addInParameter(0, itINTEGER, TR("view id"), TR("id of the view to which this drawer has to be added"));
	operation.addInParameter(1, itSTRING, TR("layer index"));
	operation.addInParameter(2, itRASTER, TR("Red Band"));
	operation.addInParameter(3, itRASTER, TR("Green Band"));
	operation.addInParameter(4, itRASTER, TR("Blue Band"));
	operation.setOutParameterCount({ 0 });
	operation.setKeywords("visualization");

	mastercatalog()->addItems({ operation });
	return operation.id();
}
