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
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/layermanager.h"
#include "coveragedisplay\/visualattribute.h"
#include "setactiveattribute.h"
#include "uicontextmodel.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(SetActiveAttribute)

SetActiveAttribute::SetActiveAttribute()
{

}

SetActiveAttribute::SetActiveAttribute(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool SetActiveAttribute::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    _layer->activeAttributeName(_attributeName);
    return true;
}

Ilwis::OperationImplementation *SetActiveAttribute::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new SetActiveAttribute(metaid, expr);
}

Ilwis::OperationImplementation::State SetActiveAttribute::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
    if (!getViewId(_expression.input<QString>(0))) {
        return sPREPAREFAILED;
    }

    _layer = layerManager()->findLayer(_expression.input<QString>(1).toInt());
    if (!_layer)
        return sPREPAREFAILED;
    bool ok;
    QString attr = _expression.parm(2).value();
    int index = attr.toUInt(&ok);
    if (!ok) {
        if (_layer->visualAttribute(attr))
            _attributeName = attr;
    } else {
        _attributeName = _layer->visualVPropertyByIndex(index);
		if (_attributeName == sUNDEF)
			_attributeName = LAYER_WIDE_ATTRIBUTE;
    }

    if (_attributeName == sUNDEF) {
        kernel()->issues()->log(TR("Could not find visual attribute for:") + attr);
        return sPREPAREFAILED;
    }


    return sPREPARED;
}

quint64 SetActiveAttribute::createMetadata()
{
    OperationResource operation({ "ilwis://operations/setactiveattribute" });
    operation.setSyntax("setactiveattribute(viewid, layerindex, attributename/index)");
    operation.setDescription(TR("Sets the active attribute of a layer to the layer indicated by its name or index"));
    operation.setInParameterCount({ 3 });
    operation.addInParameter(0, itINTEGER, TR("view id"), TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1, itSTRING, TR("layer index"));
    operation.addInParameter(2, itINT32|itSTRING, TR("attribute name/index"), TR("Which attribute should become the attribute to color the map"));
    operation.setOutParameterCount({ 0 });
    operation.setKeywords("visualization");

    mastercatalog()->addItems({ operation });
    return operation.id();
}
