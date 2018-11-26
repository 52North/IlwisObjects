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

#include <QStandardItem>
#include "kernel.h"
#include "ilwisdata.h"
#include "coverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "mastercatalog.h"
#include "uicontextmodel.h"
#include "coveragedisplay/draweroperation.h"
#include "coveragedisplay/layermanager.h"
#include "coveragedisplay/layermodel.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "../drawercolorsets.h"
#include "adddrawer.h" 

using namespace Ilwis;
using namespace Ui;

REGISTER_OPERATION(AddDrawer)

AddDrawer::AddDrawer()
{

}

AddDrawer::~AddDrawer()
{

}

AddDrawer::AddDrawer(quint64 metaid, const Ilwis::OperationExpression &expr) : DrawerOperation(metaid, expr)
{

}

bool AddDrawer::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if (_coverage.isValid()) {
        CoverageLayerModel *ldrawer = static_cast<CoverageLayerModel *>(LayerManager::create(_parentLayer, _coverage, layerManager(), _options));
        if (!ldrawer) {
            kernel()->issues()->log(TR("Couldn't create layer for :") + _coverage->resource().url().toString());
            return false;
        }

        ldrawer->coverage(_coverage);

        QVariant var = qVariantFromValue((void *)ldrawer);
        ldrawer->setActiveAttribute(0);
        ctx->addOutput(symTable, var, "layerdrawer", _coverage->ilwisType(), Resource());
        ldrawer->updateGeometry(true);
    }
    else if (_drawerCode != "") {
        LayerModel *ldrawer = LayerManager::create(_parentLayer, _drawerCode, layerManager(), _layername);
        if (!ldrawer) {
            return ERROR2(ERR_NO_INITIALIZED_2, "Drawer", _drawerCode);
        }

        QVariant var = qVariantFromValue((void *)ldrawer);
        ctx->addOutput(symTable, var, "layerdrawer", itLAYER, Resource());
        ldrawer->updateGeometry(true);
    }
    layerManager()->refresh();

    return true;
}

OperationImplementation *AddDrawer::create(quint64 metaid, const OperationExpression &expr)
{
    return new AddDrawer(metaid, expr);
}

Ilwis::OperationImplementation::State AddDrawer::prepare(ExecutionContext *ctx, const SymbolTable &t)
{
    OperationImplementation::prepare(ctx, t);
    if (!getViewId(_expression.input<QString>(0))) {
        return sPREPAREFAILED;
    }
    QString parent = _expression.input<QString>(1);
    QString source = _expression.input<QString>(2);
    _drawerCode = _expression.input<QString>(3);

    IlwisTypes tp = IlwisObject::name2Type(_drawerCode);
    if (!hasType(tp, itCOVERAGE) && source != "") {
        ERROR2(ERR_ILLEGAL_VALUE_2, TR("dataype for layer drawer"), _drawerCode);
        return sPREPAREFAILED;
    }
    if (parent != "" && parent != sUNDEF) {
        LayerIndex idx = parent.toInt();
        if (!(_parentLayer = dynamic_cast<QStandardItem *>(layerManager()->findLayer(idx)))) {
            kernel()->issues()->log(TR("The parent of the to be added layer doesnt exist:") + parent);
            return sPREPAREFAILED;
        }
    }
    else
        _parentLayer = layerManager()->layerTree()->invisibleRootItem();


    if (source != "") {
        if (source.indexOf("itemid=") != -1) {
            if (source[0] == '\"')
                source = source.mid(1, source.size() - 2);
            std::vector<Resource> res = mastercatalog()->select(source);
            if (res.size() != 1) {
                kernel()->issues()->log(QString("Could not open as %1, %2").arg(_drawerCode).arg(source));
                return sPREPAREFAILED;
            }
            _coverage.prepare(res[0]);
        }
        else {
            _coverage.prepare(source, tp);
        }
        if (!_coverage.isValid()){
            kernel()->issues()->log(TR("Failed to load:") + source);
            return sPREPAREFAILED;
        }



		if (_coverage->coordinateSystem()->isUnknown() && layerManager()->rootLayer()->screenCsy().isValid()) {
			QString mes = QString("coordinate system 'unknown' not compatible with coordinate system of the layerview");
			kernel()->issues()->log(mes, IssueObject::itWarning);
			return sPREPAREFAILED;
		}
	}
	_options["visible"] = _expression.input<bool>(4);
	QString options = _expression.input<QString>(5);
	if (options.size() > 0) {
		QStringList opts = options.split(',');
		for (QString opt : opts) {
			QStringList elems = opt.split('=');
			if (elems.size() == 2) {
				QString key = elems[0];
				QString val = elems[1];
				_options[key] = val;
			}
		}
	}

    return sPREPARED;
}

quint64 AddDrawer::createMetadata()
{
    OperationResource operation({"ilwis://operations/adddrawer"});
    operation.setSyntax("adddrawer(viewid, parent,datasource,typename,visible,options)");
    operation.setDescription(TR("adds a new drawer to the layerview identified by viewid"));
    operation.setInParameterCount({6});
    operation.addInParameter(0,itINTEGER , TR("view id"),TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1,itSTRING , TR("parent"),TR("indicates the parent of the layer. If it is empty or undefined it will be added under the global layer. Parent is a '|' delimited string of layer names"));
    operation.addInParameter(2,itSTRING , TR("Data source"),TR("The url that is used to retrieve the data for this layer. It may be empty in the case oof layers that are not based on a data source"));
    operation.addInParameter(3,itSTRING , TR("Typename"),TR("which data type is represented by this url or the type of drawer that should be created in case of layers that are not based on a datasource"));
	operation.addInParameter(4, itBOOL, TR("Visible"), TR("Should the layer initially be visible"));
    operation.addInParameter(5, itSTRING, TR("Options"), TR("The options, as comma-separated key-value pairs"));

    operation.setOutParameterCount({0});
    operation.setKeywords("visualization");

    mastercatalog()->addItems({operation});
    return operation.id();
}

//-----------------------------------------------------------------------------------------------------
REGISTER_OPERATION(AddDrawerWithName)

quint64 AddDrawerWithName::createMetadata()
{
	OperationResource operation({ "ilwis://operations/adddrawer" });
	operation.setSyntax("adddrawer(viewid, parent,datasource,typename,visible,options,layername)");
	operation.setLongName(TR("Add Layer with Name"));
	operation.setDescription(TR("adds a new drawer to the layerview identified by viewid"));
	operation.setInParameterCount({ 7 });
	operation.addInParameter(0, itINTEGER, TR("view id"), TR("id of the view to which this drawer has to be added"));
	operation.addInParameter(1, itSTRING, TR("parent"), TR("indicates the parent of the layer. If it is empty it will be added under the global layer. Parent is a '|' delimited string of layer names"));
	operation.addInParameter(2, itSTRING, TR("Data source"), TR("The url that is used to retrieve the data for this layer. It may be empty in the case oof layers that are not based on a data source"));
	operation.addInParameter(3, itSTRING, TR("Typename"), TR("which data type is represented by this url or the type of drawer that should be created in case of layers that are not based on a datasource"));
	operation.addInParameter(4, itBOOL, TR("Visible"), TR("Should the layer initially be visible"));
    operation.addInParameter(5, itSTRING, TR("Options"), TR("The options, as comma-separated key-value pairs"));
	operation.addInParameter(6, itSTRING, TR("Layer Name"), TR("The name of the layer as displayed in the UI"));

	operation.setOutParameterCount({ 0 });
	operation.setKeywords("visualization");

	mastercatalog()->addItems({ operation });
	return operation.id();
}

AddDrawerWithName::AddDrawerWithName(quint64 metaid, const Ilwis::OperationExpression &expr) : AddDrawer(metaid, expr)
{

}

OperationImplementation *AddDrawerWithName::create(quint64 metaid, const OperationExpression &expr)
{
	return new AddDrawerWithName(metaid, expr);
}

bool AddDrawerWithName::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	return AddDrawer::execute(ctx, symTable);
}


Ilwis::OperationImplementation::State AddDrawerWithName::prepare(ExecutionContext *ctx, const SymbolTable &tbl)
{
	if (AddDrawer::prepare(ctx, tbl) == sPREPAREFAILED) {
		return sPREPAREFAILED;
	}
	_layername = _expression.input<QString>(6);

	return sPREPARED;

}

//-----------------------------------------------------------------------------------------------------
REGISTER_OPERATION(AddDrawerWithNameAndTreePlace)

quint64 AddDrawerWithNameAndTreePlace::createMetadata()
{
    OperationResource operation({ "ilwis://operations/adddrawer" });
    operation.setSyntax("adddrawer(viewid, parent,datasource,typename,visible,options,layername,aboveLayer)");
    operation.setLongName(TR("Add Layer with Name"));
    operation.setDescription(TR("adds a new drawer to the layerview identified by viewid"));
    operation.setInParameterCount({ 8 });
    operation.addInParameter(0, itINTEGER, TR("view id"), TR("id of the view to which this drawer has to be added"));
    operation.addInParameter(1, itSTRING, TR("parent"), TR("indicates the parent of the layer. If it is empty it will be added under the global layer. Parent is a '|' delimited string of layer names"));
    operation.addInParameter(2, itSTRING, TR("Data source"), TR("The url that is used to retrieve the data for this layer. It may be empty in the case oof layers that are not based on a data source"));
    operation.addInParameter(3, itSTRING, TR("Typename"), TR("which data type is represented by this url or the type of drawer that should be created in case of layers that are not based on a datasource"));
    operation.addInParameter(4, itBOOL, TR("Visible"), TR("Should the layer initially be visible"));
    operation.addInParameter(5, itSTRING, TR("Options"), TR("The options, as comma-separated key-value pairs"));
    operation.addInParameter(6, itSTRING, TR("Layer Name"), TR("The name of the layer as displayed in the UI"));
    operation.addInParameter(7, itINT32, TR("Above Layer"), TR("The node id above which the layer will be placed"));

    operation.setOutParameterCount({ 0 });
    operation.setKeywords("visualization");

    mastercatalog()->addItems({ operation });
    return operation.id();
}

AddDrawerWithNameAndTreePlace::AddDrawerWithNameAndTreePlace(quint64 metaid, const Ilwis::OperationExpression &expr) : AddDrawerWithName(metaid, expr)
{

}

OperationImplementation *AddDrawerWithNameAndTreePlace::create(quint64 metaid, const OperationExpression &expr)
{
    return new AddDrawerWithNameAndTreePlace(metaid, expr);
}

bool AddDrawerWithNameAndTreePlace::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    return AddDrawerWithName::execute(ctx, symTable);
}


Ilwis::OperationImplementation::State AddDrawerWithNameAndTreePlace::prepare(ExecutionContext *ctx, const SymbolTable &tbl)
{
    if (AddDrawer::prepare(ctx, tbl) == sPREPAREFAILED) {
        return sPREPAREFAILED;
    }
    _layername = _expression.input<QString>(6);
    bool ok;
    auto lnid = _expression.input<qint32>(7, ok);
    if ( lnid  != iUNDEF)
        _options.addOption("lowernodeid", lnid);


    return sPREPARED;

}
