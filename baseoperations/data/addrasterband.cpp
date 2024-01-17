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
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "attributedefinition.h"
#include "addrasterband.h"
#include "catalog.h"
#include "ilwiscontext.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(AddRasterBand)

AddRasterBand::AddRasterBand()
{
}

AddRasterBand::AddRasterBand(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

quint32 AddRasterBand::mapIndexes(const IRasterCoverage& inRC, quint32 in, const IRasterCoverage& outRC) {
    QString v = inRC->stackDefinition().index2value(in);
    return outRC->stackDefinition().index(v);
}
void AddRasterBand::addBands()
{
    auto code = _outputRaster->stackDefinition().domain()->code() ;
    if ( code == "count"){
        int outIndex=0;
        for(size_t i=0; i < _inputRaster->size().zsize();++i){
            RasterCoverage::copyBands(_inputRaster,  _outputRaster, i, outIndex,_mergeOptions);
            ++outIndex;
        }
        for(size_t i=0; i < _bands.size();++i){
            for (size_t j=0; j < _bands[i]._band->size().zsize(); ++j){
                RasterCoverage::copyBands(_bands[i]._band,  _outputRaster, j, outIndex,_mergeOptions);
                ++outIndex;
            }
        }

    }else {
        IlwisTypes valueType = _outputRaster->stackDefinition().domain()->valueType();
        if ( hasType(valueType, itINTEGER | itSTRING | itDOUBLE) ){
            int outIndex=0;
            for(size_t i=0; i < _inputRaster->size().zsize();++i){
                RasterCoverage::copyBands(_inputRaster,  _outputRaster, i, outIndex);
                ++outIndex;
            }
            for(size_t i=0; i < _bands.size();++i){
                for (size_t j=0; j < _bands[i]._band->size().zsize(); ++j){
                    auto indx = mapIndexes(_bands[i]._band, j, _outputRaster);
                    RasterCoverage::copyBands(_bands[i]._band,  _outputRaster, indx, outIndex);
                    ++outIndex;
                }
            }
        }
    }
}

void AddRasterBand::makeValid() {
    int outIndex=0;
    for(size_t i=0; i < _inputRaster->size().zsize();++i){
        QString v = _bands[i]._band->stackDefinition().index2value(i);
        auto indx = _outputRaster->stackDefinition().index(v);
        RasterCoverage::copyBands(_inputRaster,  _outputRaster, i, indx);
        ++outIndex;
    }
    for(size_t i=0; i < _bands.size();++i){

    }
}

bool AddRasterBand::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    if ( _mergeOptions == RasterCoverage::moNONE){
        addBands();
    } else if ( _mergeOptions == RasterCoverage::moVALID){
        addBands();
    }

    //logOperation(_inputRaster, _expression, { _bands });
    setOutput(_inputRaster, ctx, symTable);

    return true;
}

void AddRasterBand::insertBand(const IRasterCoverage& mergeRaster, const QString& index){
    auto iter = mergeRaster->band(index);


}

Ilwis::OperationImplementation *AddRasterBand::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new AddRasterBand(metaid, expr);
}

Ilwis::OperationImplementation::State AddRasterBand::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString rasterUrl = _expression.input<QString>(0);
    if (!_inputRaster.prepare(rasterUrl)){
        kernel()->issues()->log(TR("Raster can not be loaded:") + rasterUrl);
        return sPREPAREFAILED;
    }
    rasterUrl = _expression.input<QString>(1);
    QStringList bands = _expression.input<QString>(1).split("|");
    for ( QString bandUrl : bands) {
        QString path = context()->workingCatalog()->resource().url().toString() + "/" + bandUrl;
        IRasterCoverage band;
        if (!band.prepare(path)){
            kernel()->issues()->log(TR("Raster can not be loaded:") + rasterUrl);
            return sPREPAREFAILED;
        }
        _bands.push_back(BandMergeInfo(band));
    }
    for(auto band : _bands){
        auto iscompat = _inputRaster->datadef().isCompatibleWith(band._band->datadef());
        if ( !iscompat){
            kernel()->issues()->log(TR("Domains of the input raster and the band are not compatible:"));
            return sPREPAREFAILED;
        }
    }


    QString mergeOption = _expression.input<QString>(2);
    if (mergeOption == "valid")
        _mergeOptions = RasterCoverage::MergeOptions::moVALID;
    else if (mergeOption == "max")
        _mergeOptions = RasterCoverage::MergeOptions::moMAX;
    else if (mergeOption == "min")
        _mergeOptions = RasterCoverage::MergeOptions::moMIN;
    else if (mergeOption == "raster1")
        _mergeOptions = RasterCoverage::MergeOptions::moRASTER1;
    else if (mergeOption == "raster2")
        _mergeOptions = RasterCoverage::MergeOptions::moRASTER2;
    else if (mergeOption == "mean")
        _mergeOptions = RasterCoverage::MergeOptions::moMEAN;
    else if (mergeOption == "none")
        _mergeOptions = RasterCoverage::MergeOptions::moNONE;
    else{
        kernel()->issues()->log(TR("Invalid merge option:" + mergeOption));
        return sPREPAREFAILED;
        }
    auto sz = _inputRaster->size();
    int zsize = sz.zsize();
    auto indexes = _inputRaster->stackDefinition().indexes();
    for(auto& band : _bands){
        auto newIndexesValues = band._band->stackDefinition().indexes();
        IlwisTypes stackType = band._band->stackDefinition().domain()->valueType();
        for(auto newIndexValue : newIndexesValues){
            int foundIdx = _inputRaster->stackDefinition().contains(newIndexValue);
            if ( foundIdx != iUNDEF && _mergeOptions != RasterCoverage::moNONE){
               band._clashes.push_back(std::pair<int, QString>(foundIdx, newIndexValue));
            }else{
                if ( foundIdx != iUNDEF){
                    if ( hasType(stackType,itINTEGER)){
                        newIndexValue = QString::number(indexes.size());
                    }
                    else if ( stackType == itDOUBLE){
                        double d = newIndexValue.toDouble() + 0.001;
                        newIndexValue = QString::number(d);
                    } else if ( stackType == itSTRING){
                        newIndexValue = newIndexValue + "_alt";
                    }
                }
                indexes.push_back(newIndexValue);
                zsize++;
            }

        }
    }

    OperationHelperRaster helper;

    sz.zsize(zsize);
    helper.initialize(_inputRaster, _outputRaster, itENVELOPE | itCOORDSYSTEM | itGEOREF | itDOMAIN);
    _outputRaster->size(sz);
    _outputRaster->setDataDefintions(_outputRaster->datadef().domain(), indexes);
    initialize(_outputRaster->size().linearSize());


    return sPREPARED;
}

quint64 AddRasterBand::createMetadata()
{

    OperationResource operation({"ilwis://operations/addrasterband"});
    operation.setSyntax("addrasterband(inputraster, band, mergeoptions=none|raster1|raster2|max|minmean|valid)");
    operation.setDescription(TR("adds a new band and returns the input raster with the extra band; The merge options will determine what happens with the new bands"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.addInParameter(1,itRASTER, TR("new band(s)"), TR("bands to be added with a domain compatible with the input raster"));
    operation.addInParameter(2,itSTRING, TR("Merge options"), TR("Describes how the bands will be merged. Some of the options describe behavior incase of clashes with index"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER , TR("output raster"),TR("raster with extra band"));
    operation.setKeywords("raster,workflow");

    operation.checkAlternateDefinition();
    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();

}

