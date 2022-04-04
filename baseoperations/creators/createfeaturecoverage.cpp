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
#include "domain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "itemdomain.h"
#include "itemiterator.h"
#include "factory.h"
#include "coverage.h"
#include "catalog.h"
#include "table.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "operationhelper.h"
#include "feature.h"
#include "featureiterator.h"
#include "tablemerger.h"
#include "parsestackdefinition.h"
#include "createfeaturecoverage.h"


using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateFeatureCoverage)

CreateFeatureCoverage::CreateFeatureCoverage()
{

}

CreateFeatureCoverage::CreateFeatureCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

Ilwis::OperationImplementation *CreateFeatureCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateFeatureCoverage(metaid, expr);
}

bool Ilwis::BaseOperations::CreateFeatureCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
	TableMerger merger;
	int count = 0;
	for (auto fc : _features) {
		FeatureIterator iterIn1(fc);
		int offset = _outputFeatures->featureCount();
		for_each(iterIn1, iterIn1.end(), [&](SPFeatureI feature) {
			_outputFeatures->newFeatureFrom(feature, fc->coordinateSystem());
            auto targetTable = _outputFeatures->attributeTable();
            merger.mergeTableData(fc->attributeTable(), targetTable, offset);
			updateTranquilizer(count++, 10);
		});
	}
	QVariant value;
	value.setValue<IFeatureCoverage>(_outputFeatures);
	logOperation(_outputFeatures, _expression);
	ctx->setOutput(symTable, value, _outputFeatures->name(), itFEATURE, _outputFeatures->resource());
    return true;
}

Ilwis::OperationImplementation::State CreateFeatureCoverage::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	OperationImplementation::prepare(ctx, st);
	auto CreateStackDomain = [&](const QString& dom)-> Ilwis::OperationImplementation::State {
		_stackDomain.prepare(dom);
		if (!_stackDomain.isValid()) {
			kernel()->issues()->log(QString(TR("%1 is and invalid stack domain")).arg(dom));
			return sPREPAREFAILED;
		}
		return sPREPARED;
	};

	QString csy = _expression.input<QString>(0);
	;
	if (!_csy.prepare(csy, { "mustexist", true })) {
		kernel()->issues()->log(QString(TR("%1 is and invalid coordinatesystem")).arg(csy));
		return sPREPAREFAILED;
	}
	QString coverages = _expression.input<QString>(1);
	if (coverages != sUNDEF && (coverages.indexOf("?") != -1 || coverages.indexOf("*") != -1)) {
		coverages = OperationHelper::expandWildCards(coverages);
	}
	int featureCount = 0;
	if (coverages != "" && coverages != sUNDEF && coverages != "default") {
		QStringList layers = coverages.split(",");
		for (QString layer : layers) {
			IFeatureCoverage fc;
			if (fc.prepare(layer,{ "mustexist", true })) {
				_features.push_back(fc);
				featureCount += fc->featureCount();
			}
			else {
				kernel()->issues()->log(QString(TR("%1 is and invalid coverage; feature coverafe can not be build")).arg(layer));
				return sPREPAREFAILED;
			}
		}
	}

	QString potentialCatalog = _expression.input<QString>(3);
	ICatalog cat;
	if (cat.prepare(potentialCatalog, { "mustexist", true })) {
		auto resources = cat->items();
		QString domainName = _expression.input<QString>(3);
		QString path = cat->resource().container().toString();
		INamedIdDomain dom;
		if (dom.prepare(domainName, { "extendedtype",itNAMEDITEM })) {
			for (auto resource : resources) {
				NamedIdentifier *item = new NamedIdentifier(resource.name());
				dom->addItem(item);
			}
			dom->connectTo(path + "/" + domainName, "domain", "stream", IlwisObject::cmOUTPUT);
			dom->store();
			_stackDomain = dom;
			for (auto layer : _features)
				_stackValueStrings.push_back(layer->name());
		}
	}
	else {
		if (!_stackDomain.prepare(_expression.input<QString>(2))) {
			_stackDomain = IDomain("count");
			_stackValueStrings = { "1" };
			_stackValueNumbers = { 1 };
		}
	}
	ParseStackDefinition parser;
	parser.parseStackDefintion(_expression.input<QString>(3), _features.size(), _stackDomain, _stackValueStrings, _stackValueNumbers);

	_outputFeatures.prepare();
	_outputFeatures->coordinateSystem(_csy);
	_outputFeatures->attributeDefinitionsRef().setSubDefinition(_stackDomain, _stackValueStrings);

	initialize(featureCount);

	TableMerger merger;
	ITable tbl;
	tbl.prepare();
	for (auto fc : _features) {
		if (!merger.mergeMetadataTables(tbl, fc->attributeTable(), std::vector<QString>())) {
			kernel()->issues()->log(TR("Column conflict while merging, maybe name or domain clash:") + fc->name());
			return sPREPAREFAILED;
		}
	}
	_mergeLayers = _expression.input<bool>(4);
	_outputFeatures->setAttributes(tbl);
    return sPREPARED;
}

quint64 CreateFeatureCoverage::createMetadata(){

    OperationResource resource({"ilwis://operations/createfeaturecoverage"});
    resource.setLongName("Create Featurer coverage");
    resource.setSyntax("createfeaturecoverage(coordinatesystem, coveragelist,stackdomain,stackdefinition,mergelayers");
    resource.setInParameterCount({5});
    resource.addInParameter(0,itCOORDSYSTEM, "Coordinate system", "Coordinatesystem used by the output feature coverage");
    resource.addInParameter(1, itSTRING,TR("Feature coverages"), TR("A list of feature coverages to be added to the new coverage. Different stack entries may be indicated by <stack-element>(<list of coverages>)"));
    resource.addInParameter(2,itDOMAIN, "Stack domain", "domain used for the stack, by default it is count");
	resource.addInParameter(3, itSTRING, "Stack definition", "Values of the stack elements");
	resource.addInParameter(4, itBOOL, "Merge Layers", "All coverages are merged into one vector layer or they will remain seperated in the stack");
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itFEATURE, TR("new feature coverage"), TR("The newly createdfeature coverage"));
    resource.setKeywords("create, feature, workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}
