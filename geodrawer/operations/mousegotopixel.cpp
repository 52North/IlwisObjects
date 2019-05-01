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

#include <QCursor>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "raster.h"
#include "rasterlayermodel.h"
#include "mousegotopixel.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(MouseGoToPixel)

MouseGoToPixel::MouseGoToPixel()
{

}

MouseGoToPixel::~MouseGoToPixel()
{

}

MouseGoToPixel::MouseGoToPixel(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool MouseGoToPixel::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	QQuickItem *drawer = uicontext()->rootObject()->findChild<QQuickItem *>(layerManager()->qmlDrawer());
	if (drawer) {
		auto pnt = drawer->mapToGlobal(QPoint(_column, _row));
		QCursor cursor;
		cursor.setPos(pnt.x(), pnt.y());
	}

	return true;
}

Ilwis::OperationImplementation *MouseGoToPixel::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new MouseGoToPixel(metaid, expr);
}

Ilwis::OperationImplementation::State MouseGoToPixel::prepare(ExecutionContext *ctx, const SymbolTable &)
{
	if (!getViewId(_expression.input<QString>(0))) {
		return sPREPAREFAILED;
	}
	auto layer = layerManager()->findLayer(_expression.input<QString>(1).toInt());
	if (!_layer) {
		kernel()->issues()->log(TR("layer doesnt exist"));
		return sPREPAREFAILED;
	}
	_layer = dynamic_cast<RasterLayerModel *>(layer);
	if (!_layer) {
		kernel()->issues()->log(TR("layer is not a raster layer"));
		return sPREPAREFAILED;
	}

	auto column = _expression.input<QString>(2).toInt();
	IRasterCoverage raster = _layer->coverage().as<RasterCoverage>();
	if (column < 0 || column >raster->size().xsize()) {
		kernel()->issues()->log(TR("column doesnt fit into the raster:") + QString::number(column));
		return sPREPAREFAILED;
	}
	_column = column;
	auto row = _expression.input<QString>(3).toInt();
	if (row < 0 || row >raster->size().ysize()) {
		kernel()->issues()->log(TR("row doesnt fit into the raster:") + QString::number(row));
		return sPREPAREFAILED;
	}


	return sPREPARED;
}

quint64 MouseGoToPixel::createMetadata()
{
	OperationResource operation({ "ilwis://operations/mousegotopixel" });
	operation.setSyntax("mousegotopixel(viewid,layer, x,y)");
	operation.setDescription(TR("Sets the mouse position on a certain col/row position"));
	operation.setInParameterCount({ 4 });
	operation.addInParameter(0, itINTEGER, TR("view id"), TR("id of the view to which this drawer has to be added"));
	operation.addInParameter(1, itINTEGER, TR("layer index"));
	operation.addInParameter(2, itINTEGER, TR("Column"));
	operation.addInParameter(3, itINTEGER, TR("Row"));
	operation.setOutParameterCount({ 0 });
	operation.setKeywords("visualization");

	mastercatalog()->addItems({ operation });
	return operation.id();
}
