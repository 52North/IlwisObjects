
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
#include "pixeliterator.h"
#include "MapFillSinks.h"

using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(MapFillSinks)

MapFillSinks::MapFillSinks()
{
}


MapFillSinks::MapFillSinks(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}

bool MapFillSinks::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
   if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    executeFillSink();

    bool resource = true;
    if (resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outRaster);
        ctx->setOutput(symTable, value, _outRaster->name(), itRASTER, _outRaster->resource());
    }
    return resource;
}

Ilwis::OperationImplementation* MapFillSinks::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new MapFillSinks(metaid, expr);
}

Ilwis::OperationImplementation::State MapFillSinks::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
	m_sinkPixels = 0;
    m_sinkHeight = -99999;
    m_sinkPixelsThreshold = 2;
	
    OperationImplementation::prepare(ctx, st);
    QString inraster = _expression.parm(0).value();
    QString outputName = _expression.parm(0, false).value();
    QString methodstr = _expression.parm(1).value().toLower();

    std::map<QString, FillMethod> methods = { {"fill",fmFill},{"cut",fmCut} };


    auto iter = methods.find(methodstr);
    if (iter == methods.end()) 
    {
        ERROR2(ERR_NOT_FOUND2, methodstr, TR("in method for fillsinks"));
        return sPREPAREFAILED;
    }
    method = iter->second;


    if (!_inRaster.prepare(inraster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, inraster, "");
        return sPREPAREFAILED;
    }
  
    //_outRaster = _inRaster->clone();

    
    int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
    _outRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
    if (!_outRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    IDomain dom("code=domain:value");
    _outRaster->datadefRef() = DataDefinition(dom);

    for (quint32 i = 0; i < _outRaster->size().zsize(); ++i) {
        QString index = _outRaster->stackDefinition().index(i);
        _outRaster->setBandDefinition(index, DataDefinition(dom));
    }

    if (outputName != sUNDEF)
        _outRaster->name(outputName);

  
    long xz = _outRaster->size().xsize();
    long yz = _outRaster->size().ysize();

    PixelIterator iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();
    
    std::vector<double> vals;
    while (iterDEM != inEnd)
    {
        *iterOut = *iterDEM;
        *iterOut++;
        *iterDEM++;
    }

    return sPREPARED;
}

quint64 MapFillSinks::createMetadata()
{
    OperationResource operation({ "ilwis://operations/MapFillSinks" });
    operation.setSyntax("MapFillSinks(inputraster,method=fill|cut)");
    operation.setDescription(TR("Remove local depressions or cut terrain from a DEM raster according to the method indicated by the second parameter"));
    operation.setInParameterCount({ 2 });
    operation.addInParameter(0, itRASTER, TR("input raster DEM"), TR("input raster DEM with numeric domain"));
    operation.addInParameter(1, itSTRING, TR("fill method"), TR("fill sinks or cut terrain"), OperationResource::ueCOMBO);
    operation.parameterNeedsQuotes(1);
    operation.setOutParameterCount({ 1 });
    //operation.addValidation(0, 1, "values with select code from filters where type='linear' ");
    operation.addOutParameter(0, itRASTER, TR("output raster"), TR("output raster DEM that will contain height values without local depressions"));
    operation.setKeywords("fill sink,raster,image processing, numeric");

    operation.checkAlternateDefinition();
    mastercatalog()->addItems({ operation });
    return operation.id();
}


////////////////////////////////////////////////////////////////////////////////////////////////

void MapFillSinks::executeFillSink()
{
    m_sinkPixelsThreshold = 2; //TO initiallize!!!
    
    /*iterFlag 1. Each element is initialled to zero;
                2. Elements in depres.contributing area are flagged,
                   when the the depres. area are defined;
                3. Elements in flat area flagged to -1*/

    _xsize = _inRaster->size().xsize();
    _ysize = _inRaster->size().ysize();
    
    quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
    _flagRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
   
    // ini flag value as 0;
    iterFlag = PixelIterator(_flagRaster, BoundingBox(), PixelIterator::fXYZ);
    std::fill(iterFlag, iterFlag.end(), 0);

    m_iFlag = 0;

    iterOut = PixelIterator(_outRaster, BoundingBox(), PixelIterator::fXYZ);
    SingleSinkFill();

    //Fill sinks based on the user specified threshold
    if (method == fmCut)
        GroupSinksFill();

    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();

    while (iterDEM != inEnd)
    {
        Pixel pxl = iterDEM.position();
        if ( !onEdge(pxl) && fLocateInitialSink(pxl) )
        {
            /*increment flag variable, locate the extent of the
                     contributing sink area*/
            m_iFlag++; //increment by the number of init. sink
            FindSinkContArea(pxl);

            //Identify outlet cell
            Pixel rcOutlet;
            if (fIdentifyOutletCell(pxl, rcOutlet))
            {
                /*if the elevation of outlet is greater than that of the
                 *initial sink then fill the depressions, otherwise
                 *if they are equal to, no filling is needed*/
                if (method == fmFill) {
                    if (getPixelValue(rcOutlet) >
                        getPixelValue(pxl))
                    {
                        DepresFill(rcOutlet);
                    }
                    else
                        FlatAreaFlag(rcOutlet);
                }
                else
                    CutTerrain(rcOutlet);
            }
            else
                FlatAreaFlag(rcOutlet);
            _vPxlSinks.clear();

        }

        iterDEM++;
    }

    _vPxlSinks.resize(0);

}


void MapFillSinks::CutTerrain(Pixel rcOutlet)
{
    //cell with lowest height to the outlet is selected for
    //breaching and filling in this case
    double cutValue = getCutValue(rcOutlet);
    double rHeight = getPixelValue(rcOutlet);
    std::deque<Pixel>::iterator pos;
    for (pos = _vPxlSinks.begin(); pos != _vPxlSinks.end(); ++pos)
    {
        if (getPixelValue(*pos) <= rHeight)
        {
            setPixelValue(*pos, cutValue);
            *(iterFlag(*pos)) = -1; //flag the cell within the sink area
            //_vFlags[pos->y][pos->x] = -1;  //flag the cell within the sink area
        }
    }
}


double MapFillSinks::getCutValue(Pixel rcOutlet)
{
    //Get the lowerest height of the neighbouring cells of the outlet
    double rHeight = getPixelValue(rcOutlet);
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (getPixelValue(Pixel(rcOutlet.x + j, rcOutlet.y + i,0)) < rHeight)
                rHeight = getPixelValue(Pixel(rcOutlet.x + j, rcOutlet.y + i,0));
        }
    }

    return rHeight;
}


void MapFillSinks::FlatAreaFlag(Pixel rcOutlet)
{
    //flag the cells in an existing flat area
    std::deque<Pixel>::iterator pos;
    for (pos = _vPxlSinks.begin(); pos != _vPxlSinks.end(); ++pos)
    {
        //_vFlags[(*pos).y][(*pos).x] = -1;
        *(iterFlag(*pos)) = -1;
    }
}


void MapFillSinks::DepresFill(Pixel rcOutlet)
{
    /*for each cell in the sink cont. area, if it is lower than the
     *elevation of the outlet, raise iis elevation to that of outlet.*/
    double rHeight = getPixelValue(rcOutlet);
    std::deque<Pixel>::iterator pos;
    for (pos = _vPxlSinks.begin(); pos != _vPxlSinks.end(); ++pos)
    {
        if (getPixelValue(*pos) < rHeight)
            setPixelValue(*pos, rHeight);

       // _vFlags[pos->y][pos->x] = -1;  //flag the cell within the sink area
        *(iterFlag(*pos)) = -1;  //flag the cell within the sink area
    }
}



void MapFillSinks::setPixelValue(Pixel pxl, double val)
{
    *(iterOut(pxl)) = val;
}


void MapFillSinks::SingleSinkFill()
{
    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();

    while (iterDEM != inEnd)
    {
        Pixel pxl = iterDEM.position();
        if (onEdge(pxl))
            *(iterFlag(pxl)) = -2;
        else if( *(iterDEM[pxl]) == rUNDEF )
                FlagNeighbors(pxl);
        iterDEM++;
    }

    iterDEM = Ilwis::begin(_inRaster); // back to start position

    std::vector<double>::iterator pos;

    while (iterDEM != inEnd)
    {
        Pixel pxl = iterDEM.position();
        if ( !onEdge(pxl) && !IsUndef(pxl))
        {
            std::vector<double> vNbs;
            GetNeighborCells(pxl, vNbs);
            pos = min_element(vNbs.begin(), vNbs.end());
            double rMin = *pos;
            if (*(iterOut(pxl)) < rMin)
                *(iterOut(pxl)) = rMin;

        }
        iterDEM++;
    }

 }


bool MapFillSinks::IsUndef(Pixel pxl)
{
    //check to see if it is a cell with undef
    if (*(iterFlag(pxl)) == -3 )
        return true;
    else
        return false;
}


void MapFillSinks::FlagNeighbors(Pixel pxl)
{
    //flag all neighbors to -1, if it is a cell with undef of elevation
      //or it has an undef neighbor
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            Pixel nbpxl(pxl.x + j, pxl.y+i, 0);
            *(iterFlag(nbpxl)) = -3;
        }
    }
}


void MapFillSinks::GetNeighborCells(Pixel pxl, std::vector<double>& vNeighbors)
{
    //Store the neighbor's value of elevation
   
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x + 1, pxl.y, 0))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x + 1, pxl.y+1,  0))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x, pxl.y+1,  0 ))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x - 1 , pxl.y+1, 0))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x - 1, pxl.y, 0))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x - 1, pxl.y-1, 0))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x,pxl.y-1, 0 ))) );
    vNeighbors.push_back( *(iterOut(Pixel(pxl.x + 1,pxl.y-1, 0))) );

}


bool MapFillSinks::fLocateInitialSink(Pixel pxl)
{
    /*check to see if cell in position rc has lower/the same elevation
   as its adjacent cells*/

   /*skiping edge cells, or
       cell has a neighbor with an undef value of elevation or
       it is already flagged*/
    if ( *(iterFlag(pxl)) == 0 )
    {
        std::vector<double>::iterator pos;
        std::vector<double> vNbs;

        GetNeighborCells(pxl, vNbs);

        pos = min_element(vNbs.begin(), vNbs.end());
        double rMin = *pos;

        if ( *(iterOut(pxl)) <= rMin )
            return true;
    }
    return false;

}


double MapFillSinks::getPixelValue(Pixel pxl)
{
    return *(iterOut(pxl));

}


void MapFillSinks::FindSinkContArea(Pixel rcInitSink)
{
    //*define the extent of sink contributing area
    std::deque<Pixel> vStartCells;
    FlagAdjaCell(rcInitSink, vStartCells);

    /*identify the outward adjacent cells for each of starting cells
     *put the located cells in vrcOutwardCells
     *flag the located cells in vSinkFlagged*/

    std::deque<Pixel>::iterator pos;
    std::deque<Pixel> vAdjaCells;
    *(iterFlag(rcInitSink)) = m_iFlag;
    //_vFlags[rcInitSink.y][rcInitSink.x] = m_iFlag;
    _vPxlSinks.resize(0);
    _vPxlSinks.push_back(rcInitSink);
    do
    {
        vAdjaCells.resize(0); /*stores all located outward adjacent cells
                                                to each of starting cell*/

        for (pos = vStartCells.begin(); pos != vStartCells.end(); ++pos)
        {
            Pixel pxl(*pos);
            _vPxlSinks.push_back(pxl);
            FlagAdjaCell(*pos, vAdjaCells);
        }
        vStartCells.swap(vAdjaCells);
    } while (vAdjaCells.size() != 0); //no more element can be located
}


void MapFillSinks::FindSinkContArea2(Pixel rcInitSink)
{
    //*define the extent of sink contributing area
    std::deque<Pixel> vStartCells;
    m_sinkPixels = 1;
    FlagAdjaCell(rcInitSink, vStartCells);

    /*identify the outward adjacent cells for each of starting cells
     *put the located cells in vrcOutwardCells
     *flag the located cells in vSinkFlagged*/

    std::deque<Pixel>::iterator pos;
    std::deque<Pixel> vAdjaCells;
    *(iterFlag(rcInitSink)) = m_iFlag;
    _vPxlSinks.resize(0);
    _vPxlSinks.push_back(rcInitSink);
    do
    {
        vAdjaCells.resize(0); /*stores all located outward adjacent cells
                                                to each of starting cell*/

        for (pos = vStartCells.begin(); pos != vStartCells.end(); ++pos)
        {
            Pixel pxl(*pos);
            _vPxlSinks.push_back(pxl);
            FlagAdjaCell(*pos, vAdjaCells);
        }
        vStartCells.swap(vAdjaCells);
        if (m_sinkPixels > m_sinkPixelsThreshold)
            break;
    } while (vAdjaCells.size() != 0); //no more element can be located
}



void MapFillSinks::FlagAdjaCell(Pixel rcStartCell, std::deque<Pixel>& vAdj)
{
    /*check and skip if a cell
    *is undefined or
    *lies at the edge of DEM or
    *is adjacent to an undefined element
    *is already existed in the vector*/

    Pixel adj;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            adj.x = rcStartCell.x + j; //adjacent to starting cell rcCell
            adj.y = rcStartCell.y + i;
            adj.z = 0;
            int ival = *(iterFlag(adj));

            if ( *(iterFlag(adj)) != m_iFlag &&
                *(iterFlag(adj)) > -2 &&
                getPixelValue(adj) >= getPixelValue(rcStartCell))
            {
                vAdj.push_back(adj);
                *(iterFlag(adj)) = m_iFlag;
                ival = *(iterFlag(adj));

                if (getPixelValue(adj) == m_sinkHeight)
                    m_sinkPixels++;
            }
        }
    }
}


class RowColValueLessClass //compare two elements for min_element algorithm
{
public:
    RowColValueLessClass(PixelIterator& iterOut) :
        iterCmpDEM(iterOut)
    {
    }
    bool operator()(Pixel pxl1, Pixel pxl2)
    {
        return *(iterCmpDEM(pxl1)) < *(iterCmpDEM(pxl2));
    }
private:
    PixelIterator &iterCmpDEM;
};


bool MapFillSinks::fIdentifyOutletCell(Pixel rcSink, Pixel& rcOutlet)
{
    //Find outlet cell in the rim of the sink contributing area
    std::deque<Pixel> vOutlets; //potential outlets
    vOutlets.resize(0);

    std::deque<Pixel>::iterator pos = _vPxlSinks.begin();
    double minval(.0);
    bool hasOuetlet(false);

    for (; pos != _vPxlSinks.end(); ++pos)
    {
        if (IsPotentialOutlet(*pos))
            vOutlets.push_back(*pos);
    }

    if (vOutlets.size() > 0)
	{
			/*the lowest outlet is selected.
			 *if two or more potential outlets have the lowest elevation
			 *the one encounted first is selected*/
			RowColValueLessClass rvl(iterOut);
			pos = min_element(vOutlets.begin(), vOutlets.end(), rvl);
			rcOutlet = *pos;
			return true;
	}
	else
	    return false;    

}




bool MapFillSinks::IsPotentialOutlet(Pixel pxl)
{
    /*Check if the current cell lies at the rim of the sink cont. area, and
     *is higher than a cell, which is outside of the arae.*/

    double rHeight = getPixelValue(pxl);
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            Pixel pospxl(pxl.x + j, pxl.y + i,0);
            if (*(iterFlag(pospxl)) != m_iFlag)
            {
                if (*(iterFlag(pospxl)) == -2)	//flat area lies at the edge of DEM
                {
                    if (rHeight >= getPixelValue(pospxl))
                        return true;
                }
                else
                {
                    if (rHeight > getPixelValue(pospxl))
                        return true;
                }
            }
        }
    }

    return false;
}

bool MapFillSinks::onEdge(Pixel pix) {
    return pix.x == 0 || pix.x == _xsize - 1 ||
        pix.y == 0 || pix.y == _ysize - 1;
}


//Fill sinks based on the threshold -  Nr. of pixels specified
void MapFillSinks::GroupSinksFill()
{
    //scan DEM seeking an initial sink

    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();

  
    while (iterDEM != inEnd)
    {
        Pixel rcSink = iterDEM.position();
        if ( !onEdge(rcSink) && fLocateInitialSink(rcSink) )
        {
            m_sinkHeight = getPixelValue(rcSink);
            /*increment flag variable, locate the extent of the
            contributing sink area*/
            m_iFlag++; //increment by the number of init. sink
            FindSinkContArea2(rcSink);
            if (m_sinkPixels <= m_sinkPixelsThreshold)
            {
                //Identify outlet cell
                Pixel rcOutlet;
                if (fIdentifyOutletCell(rcSink, rcOutlet))
                {
                    /*if the elevation of outlet is greater than that of the
                     *initial sink then fill the depressions, otherwise
                     *if they are equal to, no filling is needed*/
                    if ( getPixelValue(rcOutlet) > getPixelValue(rcSink) )
                        DepresFill(rcOutlet);
                    else
                        FlatAreaFlag(rcOutlet);
                }
                else
                    FlatAreaFlag(rcOutlet);
                _vPxlSinks.clear();
            }
        }
        iterDEM++;
    }

    iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
    while (iterDEM != inEnd)
    {
        Pixel pxl = iterDEM.position();
        if (*(iterFlag(pxl)) > 0 )
            *(iterFlag(pxl)) = 0;
        iterDEM++;
    }

}