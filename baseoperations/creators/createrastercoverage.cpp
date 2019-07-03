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
#include "raster.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemiterator.h"
#include "pixeliterator.h"
#include "catalog.h"
#include "ilwisoperation.h"
#include "operationhelper.h"
#include "operationhelpergrid.h"
#include "parsestackdefinition.h"
#include "createrastercoverage.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateRasterCoverage)

CreateRasterCoverage::CreateRasterCoverage()
{

}

CreateRasterCoverage::CreateRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateRasterCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;


    initialize(_outputRaster->size().linearSize());
    double minv=1e307,maxv = -1e307;
    PixelIterator pout(_outputRaster);
    double resolution = 1e30;
    for(auto band : _bands){
        if (_autoresample && !band->georeference()->isCompatible(_grf)) {
            band = OperationHelperRaster::resample(band, _grf);
        }
        resolution = std::min(band->datadefRef().domain()->range<NumericRange>()->resolution(), resolution);
        for(double value : band){
            *pout = value;
            minv = Ilwis::min(value,minv);
            maxv = Ilwis::max(value,maxv);
            updateTranquilizer(pout.linearPosition(), 1000);
            ++pout;
        }
    }
    if (resolution == 1e30) resolution = 0;
	NumericRange *rng;
	if (minv == 1e307 && maxv == -1e307) {
		rng = new NumericRange();
	}else
		rng = new NumericRange(minv, maxv, resolution);
    _outputRaster->datadefRef().range(rng);


    QVariant value;
    value.setValue<IRasterCoverage>(_outputRaster);
	logOperation(_outputRaster, _expression, { _grf, _domain, _stackDomain });
    ctx->setOutput(symTable,value,_outputRaster->name(),itRASTER,_outputRaster->resource());

    return true;
}

Ilwis::OperationImplementation *CreateRasterCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateRasterCoverage(metaid, expr);
}

Ilwis::OperationImplementation::State CreateRasterCoverage::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    auto CreateStackDomain = [&](const QString& dom)-> Ilwis::OperationImplementation::State{
        _stackDomain.prepare(dom);
        if ( !_stackDomain.isValid()){
            kernel()->issues()->log(QString(TR("%1 is an invalid stack domain")).arg(dom));
            return sPREPAREFAILED;
        }
        return sPREPARED;
    };

    QString grf = _expression.input<QString>(0);
    _grf.prepare(grf);
    if ( !_grf.isValid()){
        kernel()->issues()->log(QString(TR("%1 is an invalid georeference")).arg(grf));
        return sPREPAREFAILED;
    }
    QString maps = _expression.input<QString>(2);
    if ( maps != sUNDEF && (maps.indexOf("?") != -1 || maps.indexOf("*") != -1)){
        maps = OperationHelper::expandWildCards(maps);
    }
    if (maps != "" && maps != sUNDEF && maps != "default"){
        QStringList bands = maps.split(",");
        for(QString band : bands){
            IRasterCoverage raster(band);
            if ( raster.isValid() && ( raster->georeference()->isCompatible(_grf) || _autoresample)){
                _bands.push_back(raster);
                _ranges.push_back(raster->datadef().range()->clone());
            }else{
                kernel()->issues()->log(QString(TR("%1 is an invalid band; raster can not be build")).arg(raster->name()));
                return sPREPAREFAILED;
            }
        }
    }

    QString dom = _expression.input<QString>(1);
    if ( dom == "as bands"){
        if (_bands.size() > 0)
            dom = _bands[0]->datadef().domain()->resource().url().toString();
        else
            dom = "value";
    }
    _domain.prepare(dom);
    if ( !_domain.isValid()){
        kernel()->issues()->log(QString(TR("%1 is an invalid domain")).arg(dom));
        return sPREPAREFAILED;
    }
    QString potentialCatalog = _expression.input<QString>(4);
    ICatalog cat;
	if (potentialCatalog.indexOf("://") > 0 && cat.prepare(potentialCatalog, { "mustexist", true })) {
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
			for (auto band : _bands)
				_stackValueStrings.push_back(band->name());
		}
	}
    else {
        _stackDomain = IDomain("positiveInteger");
        _stackValueStrings = { "1" };
        _stackValueNumbers = { 1 };
    }
    if ( _expression.parameterCount() >= 4){
        for(int i=3; i < _expression.parameterCount(); ++i){
            if ( hasType(_expression.parm(i).valuetype(),itSTRING|itINTEGER)){
				ParseStackDefinition parser;
                parser.parseStackDefintion(_expression.input<QString>(i), _bands.size(), _stackDomain,_stackValueStrings, _stackValueNumbers);
            }
            else if ( hasType(_expression.parm(i).valuetype(),itDOMAIN)){
                if (CreateStackDomain(_expression.input<QString>(i)) == sPREPAREFAILED)
                    return sPREPAREFAILED;
            }
            else if ( hasType(_expression.parm(i).valuetype(),itBOOL) && maps.size() != 0){
                _autoresample = _expression.input<bool>(i);
            }
        }
    }


    _outputRaster.prepare();
    _outputRaster->georeference(_grf);
    _outputRaster->size( Size<>(_grf->size().xsize(), _grf->size().ysize(), std::max(_stackValueNumbers.size(), _stackValueStrings.size())));
    if ( _stackDomain->ilwisType() == itNUMERICDOMAIN)
        _outputRaster->setDataDefintions(_domain, _stackValueNumbers , _stackDomain);
    else
         _outputRaster->setDataDefintions(_domain, _stackValueStrings, _stackDomain);

    for(quint32 i=0; i < _ranges.size(); ++i){
        if ( i < _outputRaster->size().zsize()){
           _outputRaster->datadefRef(i).range(_ranges[i]);
        }

    }
    initialize(_outputRaster->size().linearSize());
    return sPREPARED;
}



quint64 CreateRasterCoverage::createMetadata()
{
     OperationResource resource({"ilwis://operations/createrastercoverage"});
     resource.setLongName("Create Raster Coverage");
     resource.setSyntax("createrastercoverage(georeference, domain,bands,stackdomain=positiveInteger, stack-defintion=1..,auto-resample)");
     resource.setInParameterCount({6});
     resource.addInParameter(0, itGEOREF,TR("Georeference"), TR("Geometry of the new rastercoverage"));
     resource.addInParameter(1, itDOMAIN|itSTRING,TR("Domain"), TR("Domain used by the raster coverage"));
     resource.addInParameter(2, itSTRING, TR("Bands"), TR("Parameter defining the bands that will be copied to the new raster coverage. Note that this parameter maybe left empty to create an empty raster"));
     resource.addInParameter(3, itDOMAIN | itSTRING,TR("Stack domain"), TR("Optional domain of the z direction (stack), default is count"));
     resource.addInParameter(4, itSTRING|itINTEGER|itRASTER,TR("Stack definition"), TR("Content of the stack, numbers, elements of item domain, raster bands or sets of numbers"));
     resource.addInParameter(5, itBOOL,TR("Auto resample"), TR("Check this option to automatically resample all bands to the input georeference"));
     resource.setOutParameterCount({1});
     resource.addOutParameter(0, itRASTER, TR("Raster coverage"), TR("The newly created raster"));
     resource.setKeywords("raster,create,workflow");

     mastercatalog()->addItems({resource});
     return resource.id();
}
//-----------------------------------------------------------------------------------------------------
REGISTER_OPERATION(CreateSimpelRasterCoverage)

quint64 CreateSimpelRasterCoverage::createMetadata()
{
    OperationResource resource({"ilwis://operations/createrastercoverage"});
    resource.setLongName("Create Simpel Raster coverage");
    resource.setSyntax("createrastercoverage(georeference, empty=!true|false)");
    resource.setDescription(TR("Creates a empty raster based on the provided georeference or rastercoverage. In case of a georeference the newly created rastercoverage will have a numeric domain; otherwise the input rastercoverage will provide both the georeference and the domain"));
    resource.setInParameterCount({2});
    resource.addInParameter(0, itGEOREF|itRASTER,TR("Georeference"), TR("Geometry of the new rastercoverage"));
    resource.addInParameter(1, itBOOL,TR("Clean"), TR("Check this option to create the raster without layers, otherwise a single empty layer will be created"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itRASTER, TR("Raster coverage"), TR("The newly created raster"));
    resource.setKeywords("raster,create,workflow");

    mastercatalog()->addItems({resource});
    return resource.id();
}

OperationImplementation *CreateSimpelRasterCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateSimpelRasterCoverage( metaid, expr);
}

CreateSimpelRasterCoverage::CreateSimpelRasterCoverage(quint64 metaid,const Ilwis::OperationExpression &expr) : CreateRasterCoverage(metaid, expr)
{}

Ilwis::OperationImplementation::State CreateSimpelRasterCoverage::prepare(ExecutionContext *ctx,const SymbolTable&){
    IRasterCoverage inputRaster;
    if ( _expression.parm(0).valuetype() == itRASTER){
        if(!inputRaster.prepare(_expression.input<QString>(0))){
            kernel()->issues()->log(TR("Invalid raster used to create new raster:") + _expression.input<QString>(0));
            return sPREPAREFAILED;
        }
        _outputRaster = OperationHelperRaster::initialize(inputRaster,itRASTER,itRASTERSIZE|itDOMAIN|itCOORDSYSTEM|itGEOREF);
        return sPREPARED;
    }
    QString grf = _expression.input<QString>(0);
    if ( ! _grf.prepare(grf)){
        kernel()->issues()->log(QString(TR("%1 is and invalid georeference")).arg(grf));
        return sPREPAREFAILED;
    }
     _empty = _expression.input<bool>(1);
    _domain.prepare("code=domain:value");
    _stackDomain = IDomain("count");
    if (!_empty){
        _stackValueStrings = {"1"};
        _stackValueNumbers = {1};
    }
    _outputRaster.prepare();
    _outputRaster->georeference(_grf);
    _outputRaster->setDataDefintions(_domain, _stackValueNumbers , _stackDomain);

    return sPREPARED;
}


bool CreateSimpelRasterCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    setOutput(_outputRaster,ctx,symTable);
    logOperation(_outputRaster, _expression);

    return true;
}
