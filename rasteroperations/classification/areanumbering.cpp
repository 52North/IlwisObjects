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

#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "blockiterator.h"
#include "domainitem.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "basetable.h"
#include "flattable.h"
#include "ilwis3buf.h"

using namespace Ilwis;
using namespace Ilwis3;

#include "areanumbering.h"

using namespace RasterOperations;

REGISTER_OPERATION(AreaNumbering)

Ilwis::OperationImplementation *AreaNumbering::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new AreaNumbering(metaid, expr);
}

AreaNumbering::AreaNumbering() :
_f8Connected(true), _fUseForPolFromRas(false)
{
}

AreaNumbering::AreaNumbering(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr),
    _f8Connected(true), _fUseForPolFromRas(false)
{
}

long AreaNumbering::iAreaNumber(double rValue, long& iArn, long iLastColPlus1, long iCount)
{
    long j, iTempArn, iSta, iEnd, iTempArnChange, iTempArnNew;

    // j = current col of the segment
    // iTempArn = temporary arenumbering of the current segment
    // iSta = start of current segment
    // iEnd = end of current segment
    // iTempArnChange = temporary store of the areanumbering to change
    // iTempArnNew = temporary store of the new areanumbering

    iSta = iLastColPlus1 - iCount;
    if (_f8Connected && !_fUseForPolFromRas)
        iSta--;
    iEnd = iLastColPlus1 - 1;
    if (_f8Connected && !_fUseForPolFromRas)
        iEnd++;
    iTempArn = 0;
    j = iSta;
    if (_fUseForPolFromRas && _f8Connected) {
        if (rValue == rPrevLine[j - 1]) { // check pixel to upper left
            iTempArn = iPrevArnLine[j - 1];
            if ((rCurrLine[j - 1] == rPrevLine[j]) && (rPrevLine[j] > rValue))
                // if pixel to left and pixel to upper right are the same
                // and the pixel values are larger than the current
                iTempArn = 0;
        }
        if (rValue == rPrevLine[iLastColPlus1]) {
            // pixel to upper right of subline equals subline pixel values
            if (!((rCurrLine[iLastColPlus1] == rPrevLine[iLastColPlus1 - 1]) &&
                (rCurrLine[iLastColPlus1] > rValue))) {
                // if pixel to right and pixel to upper left are the same
                // and the pixel values are larger than the current then they
                // will be connected and not this combination
                if (iTempArn == 0)
                    iTempArn = iPrevArnLine[iLastColPlus1];
                else
                    iEnd = iLastColPlus1;
            }
        }
    }

    for (j = iSta; j <= iEnd; j++) {
        if (rValue == rPrevLine[j]) {
            if (iTempArn == 0)  // take area number
                iTempArn = iPrevArnLine[j];
            else
                if (iTempArn != iPrevArnLine[j]) {
                    // have to list change combination
                    if (iTempArn > iPrevArnLine[j]) {
                        iTempArnChange = iTempArn;
                        iTempArnNew = iPrevArnLine[j];
                        iTempArn = iPrevArnLine[j];
                    }
                    else {
                        iTempArnChange = iPrevArnLine[j];
                        iTempArnNew = iTempArn;
                    }
                    while (1) {
                        if (ArnToBeReplacedWith[iTempArnChange] == 0) {
                            ArnToBeReplacedWith[iTempArnChange] = iTempArnNew;
                            break;                // stores new change
                        }
                        if (ArnToBeReplacedWith[iTempArnChange] == iTempArnNew)
                            break;   // change combination exists already

                        // creates new change combination with the new areanumbering values.
                        // Case 1: if 8->3 comb. exists already, and 8->5 is new change comb,
                        // then new change comb. 5->3 is added, and change comb. 8->3 remains.
                        // Case 2: if 9->5 comb. exists already, and 9->2 is new change comb,
                        // then new change comb. 5->2 is added, and change comb. 9->5 is
                        // replaced by 9->2.
                        // The search continues until it could store a change comb. or an already
                        // existing is found.

                        if (ArnToBeReplacedWith[iTempArnChange] < iTempArnNew) {
                            long h = iTempArnChange;
                            iTempArnChange = iTempArnNew;
                            iTempArnNew = ArnToBeReplacedWith[h];
                        }
                        else {
                            long h = iTempArnChange;
                            iTempArnChange = ArnToBeReplacedWith[iTempArnChange];
                            ArnToBeReplacedWith[h] = iTempArnNew;
                        }
                    }
                }
        }
    }

    if (iTempArn == 0) { // no connection found: create new number
        iTempArn = ++iArn;
        ArnToBeReplacedWith.push_back(0L);
        ArnRealAtt.push_back(rValue);
    }
    return iTempArn;
}

bool AreaNumbering::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx,symTable)) != sPREPARED)
            return false;

	long l, iLastColPlus1;
	long iArn, iTempArn, iCount;

    IRasterCoverage inRaster (_inputObj.as<RasterCoverage>()->clone());
    IRasterCoverage outRaster(_outputObj.as<RasterCoverage>()->clone());

	// l = current line
	// iLastColPlus1 = Col after final col of the current line track
	// iTempArn = temporary area number
	// iCount = width of the current line track
	// iArn = number of the current areanumbering

	iArn = 0;
    long iCols = inRaster->size().xsize();
    long iLines = inRaster->size().ysize();
	iArnLine.Size(iCols, 1, 1);
	iPrevArnLine.Size(iCols, 1, 1);
	ArnToBeReplacedWith.push_back(0);
	ArnRealAtt.push_back(rUNDEF);
	long i;
	for (i = -1; i <= iCols; i++)
		iPrevArnLine[i] = 0;
	for (i = -1; i <= iCols; i++)
		iArnLine[i] = iUNDEF;
    trq()->inform(TR("Find Areas\n"));
	// use real values
	double rValue;
	rCurrLine.Size(iCols, 1, 1);
	rPrevLine.Size(iCols, 1, 1);
	for (i = -1; i <= iCols; i++)
		rPrevLine[i] = rUNDEF;
    initialize(iLines);
    PixelIterator iterIn = PixelIterator(inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterIn2 = PixelIterator(inRaster, BoundingBox(), PixelIterator::fXYZ); // for writing back; instead of filTemp
    PixelIterator inEnd = iterIn.end();

	for (l = 0; l < iLines; l++) {
        for (i = 0; i < iCols; ++i) {
            rCurrLine[i] = *iterIn;
            iterIn++;
        }
		if (_fUseForPolFromRas) // undefined (rUNDEF) should also become areas
			// so make sure the code further on will not find the rUNDEF (in this case change it to iUNDEF, which is the value that we want it to have)
			for (i = 0; i < iCols; i++)
				if (rCurrLine[i] == rUNDEF)
					rCurrLine[i] = iUNDEF;
		rCurrLine[-1] = rUNDEF;
		rCurrLine[iCols] = rUNDEF;
		rValue = rCurrLine[0];
		iCount = 1;
		for (iLastColPlus1 = 1; iLastColPlus1 < iCols; iLastColPlus1++) {
			if (rValue == rCurrLine[iLastColPlus1])
				iCount++;
			else {
				if (rValue != rUNDEF)
					iTempArn = iAreaNumber(rValue, iArn, iLastColPlus1, iCount);
				else
					iTempArn = iUNDEF;
				for (i = iLastColPlus1 - iCount; i < iLastColPlus1; i++)
					iArnLine[i] = iTempArn;
				rValue = rCurrLine[iLastColPlus1];
				iCount = 1;
			}
		}
		if (rValue != rUNDEF)
			iTempArn = iAreaNumber(rValue, iArn, iLastColPlus1, iCount);
		else
			iTempArn = iUNDEF;
		for (i = iLastColPlus1 - iCount; i < iLastColPlus1; i++)
			iArnLine[i] = iTempArn;

        for (i = 0; i < iCols; ++i) {
            *iterIn2 = iArnLine[i];
            iterIn2++;
        }

        trq()->update(1);

		Swap(rPrevLine, rCurrLine);
		Swap(iPrevArnLine, iArnLine);
		iPrevArnLine[-1] = 0;
		iPrevArnLine[iCols] = 0;
	}
	rCurrLine.Size(0);
	rPrevLine.Size(0);

    trq()->update(1);

	//final areanumbering
	long iFinalArn = 1; // final area number
	// determine nr of final areas
	for (iTempArn = 1; iTempArn <= iArn; iTempArn++)
		if (ArnToBeReplacedWith[iTempArn] == 0)
			iFinalArn++;

    IndexedIdentifierRange* idrange = new IndexedIdentifierRange("Area", iFinalArn - 1);
    _arnDomain->range(idrange);

    IFlatTable newTable;
    newTable.prepare();
    newTable->addColumn(QFileInfo(inRaster->name()).baseName(), inRaster->datadef().domain<>());
    newTable->addColumn("npix", IlwisObject::create<IDomain>("count"));
    //false in case ofunrealistic or unreliable pixsize (e.g. latlon) or no georef att all
    double rPixelSize = inRaster->georeference()->isValid() ? inRaster->georeference()->pixelSize() : rUNDEF;
    bool fLatLon = inRaster->georeference()->isValid() && inRaster->georeference()->coordinateSystem()->isValid() && inRaster->georeference()->coordinateSystem()->isLatLon();
    bool fKnownPixelSize = !(fLatLon || rPixelSize == rUNDEF);
    if (fKnownPixelSize)
        newTable->addColumn("Area", IlwisObject::create<IDomain>("value"));

	trq()->inform(TR("Create attribute table\n"));
	iFinalArn = 1;
    initialize(iArn);
	for (iTempArn = 1; iTempArn <= iArn; iTempArn++) {
		if (ArnToBeReplacedWith[iTempArn] == 0) {
            newTable->setCell(0, iFinalArn - 1, QVariant(ArnRealAtt[iTempArn] != iUNDEF ? ArnRealAtt[iTempArn] : Ilwis::iUNDEF));
			ArnToBeReplacedWith[iTempArn] = iFinalArn++;   // add new areanumbering value
		}
		else
			ArnToBeReplacedWith[iTempArn] = ArnToBeReplacedWith[ArnToBeReplacedWith[iTempArn]]; // take final areanumbering
        trq()->update(1);
	}
	trq()->inform(TR("Renumber\n"));
	// renumber
	long iAreaNum;
    std::vector<long> aiNrPix;
	aiNrPix.resize(iArn);
    std::vector<double> arArea;
	arArea.resize(iArn);
	double rPixelArea;

	if (fKnownPixelSize)
		rPixelArea = rPixelSize * rPixelSize; // areas are quadratic

	for (i = 0; i < iArn; i++)
	{
		aiNrPix[i] = 0;
		arArea[i] = 0;
	}
    initialize(iLines);
    iterIn = PixelIterator(inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterOut = PixelIterator(outRaster, BoundingBox(), PixelIterator::fXYZ);
	for (l = 0; l < iLines; l++) {
        for (i = 0; i < iCols; ++i) {
            iArnLine[i] = *iterIn;
            iterIn++;
        }
		for (i = 0; i < iCols; i++) {
			if (iArnLine[i] != iUNDEF) {
				iArnLine[i] = ArnToBeReplacedWith[iArnLine[i]];
				iAreaNum = iArnLine[i];
				aiNrPix[iAreaNum - 1]++;
				if (fKnownPixelSize)
					arArea[iAreaNum - 1] += rPixelArea;
			}
		}
        for (i = 0; i < iCols; ++i) {
            *iterOut = iArnLine[i] != iUNDEF ? iArnLine[i] - 1 : rUNDEF; // shift values by 1; original ilwis3 code assumes iRaws
            iterOut++;
        }
        trq()->update(1);
	}

	for (i = 0; i < iFinalArn - 1; i++)
        newTable->setCell(1, i, QVariant(aiNrPix[i]));
	if (fKnownPixelSize)
		for (i = 0; i < iFinalArn - 1; i++)
            newTable->setCell(2, i, QVariant(arArea[i]));

    trq()->update(1);
	iArnLine.Size(0);
	iPrevArnLine.Size(0);
	ArnToBeReplacedWith.resize(0);

    outRaster->setAttributes(newTable);

    QVariant value;
    value.setValue<IRasterCoverage>(outRaster);
	logOperation(outRaster, _expression, { _inputObj });
    ctx->setOutput(symTable,value,outRaster->name(), itRASTER, outRaster->resource() );
    return true;
}

Ilwis::OperationImplementation::State AreaNumbering::prepare(ExecutionContext *ctx, const SymbolTable &st )
{
    OperationImplementation::prepare(ctx,st);
    QString raster = _expression.parm(0).value();
    QString outputName = _expression.parm(0,false).value();
    int copylist = itCOORDSYSTEM | itGEOREF;

    if (!_inputObj.prepare(raster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,raster,"");
        return sPREPAREFAILED;
    }
    bool ok;
    unsigned int connectivity = _expression.parm(1).value().toUInt(&ok);
    if ( !ok || ( !(connectivity == 4 || connectivity == 8))) {
        ERROR2(ERR_ILLEGAL_VALUE_2, "parameter value", "connectivity number");
        return sPREPAREFAILED;
    }
    _f8Connected = (connectivity == 8);
    if (_expression.parameterCount() == 3) {
        QString choice = _expression.input<QString>(2);
        _fUseForPolFromRas = choice.toLower() == "yes" || choice.toLower() == "true";
    }
     _outputObj = OperationHelperRaster::initialize(_inputObj,itRASTER, copylist);
    if ( !_outputObj.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IRasterCoverage outputRaster = _outputObj.as<RasterCoverage>();
    if ( outputName != sUNDEF)
        _outputObj->name(outputName);

    QString outputBaseName = outputName;
    int index = 0;
    if ( (index = outputName.lastIndexOf(".")) != -1) {
        outputBaseName = outputName.left(index);
    }

    _arnDomain.prepare();
    //_arnDomain->name(outputBaseName);
    _arnDomain->name("areanumbering");
    _arnDomain->range(new IndexedIdentifierRange());
    DataDefinition def(_arnDomain);
    outputRaster->datadefRef() = def;
    for (int band = 0; band < outputRaster->size().zsize(); ++band) {
        outputRaster->datadefRef(band) = def;
    }

    initialize(outputRaster->size().linearSize());

    return sPREPARED;
}

quint64 AreaNumbering::createMetadata()
{
    OperationResource operation({"ilwis://operations/areanumbering"});
    operation.setLongName("Area Numbering");
    operation.setSyntax("areanumbering(inputgridcoverage,connectivity=!4|8)");
    operation.setDescription(TR("Area numbering assigns unique pixel values in an output map for connected areas (areas consisting of pixels with the same value, class name, or ID)"));
    operation.setInParameterCount({2,3});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("rastercoverage with domain item, boolean or identifier domain"));
    operation.addInParameter(1,itUINT8 , TR("Connectivity"),TR("Connected cells, may be 4 or 8"));
    operation.addOptionalInParameter(2, itBOOL, TR("Number for UNDEF"), TR("Create an area for UNDEF pixels as well, making the output suitable for raster2polygon"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("output rastercoverage"),TR("output rastercoverage with the identifier domain"));
    operation.setKeywords("raster,classification");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({operation});
    return operation.id();
}


