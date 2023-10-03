
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
#include "conventionalcoordinatesystem.h"
#include "ellipsoid.h"
#include "table.h"
#include "symboltable.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"

#include "featurecoverage.h"
#include "table.h"
#include "feature.h"
#include "featureiterator.h"
#include "conventionalcoordinatesystem.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/linestring.h"
#include "coordinatedomain.h"
#include "ellipsoid.h"
#include "itemdomain.h"
#include "thematicitem.h"


#include "MapCatchmentExtraction.h"

using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(MapCatchmentExtraction)

const double rDefaultEarthRadius = 6371007.0;

using namespace std;
//
//static CoordSystem csyLamCyl(FileName fn)
//{
//    CoordSystem csy;
//    FileName fnCoordSyst(fn, ".csy", true);
//
//    CoordSystemProjection* cspr = new CoordSystemProjection(fnCoordSyst, 1);
//    csy.SetPointer(cspr);
//    cspr->datum = new MolodenskyDatum("WGS 1984", "");
//    cspr->ell = cspr->datum->ell;
//
//    Projection prj = cspr->prj;
//    String sPrj("Lambert Cylind EqualArea");
//    prj = Projection(sPrj, cspr->ell);
//    cspr->prj = prj;
//
//    return csy;
//}
//
static double rSphericalDistance(double rRadius, const LatLon& ll_1, const LatLon& ll_2)
{
	if (ll_1.Lat() == rUNDEF || ll_1.Lon() == rUNDEF || ll_2.Lat() == rUNDEF || ll_2.Lon() == rUNDEF)
		return rUNDEF;
	double phi1 = ll_1.Lat() * M_PI / 180.0; //conversion to radians
	double lam1 = ll_1.Lon() * M_PI / 180.0;
	double phi2 = ll_2.Lat() * M_PI / 180.0; ;
	double lam2 = ll_2.Lon() * M_PI / 180.0; ;
	double sinhalfc = fabs(sin((phi2 - phi1) / 2) * sin((phi2 - phi1) / 2) +
		cos(phi1) * cos(phi2) * sin((lam2 - lam1) / 2) * sin((lam2 - lam1) / 2));
	sinhalfc = sqrt(sinhalfc);
	double c; // the shortest spherical arc
	if (sinhalfc < sqrt(2.0) / 2)
		c = 2.0 * asin(sinhalfc);
	else
	{
		phi2 = -phi2;
		lam2 = M_PI + lam2;
		sinhalfc = fabs(sin((phi2 - phi1) / 2) * sin((phi2 - phi1) / 2) +
			cos(phi1) * cos(phi2) * sin((lam2 - lam1) / 2) * sin((lam2 - lam1) / 2));
		sinhalfc = sqrt(sinhalfc);
		c = M_PI - 2.0 * asin(sinhalfc);
	}
	return c * rRadius;
}

//static void VerifyColumns(Map mpMap)
//{
//
//    Table tblAtt = mpMap->tblAtt();
//    if (!tblAtt.fValid())
//        throw ErrorNoAttTable(mpMap->fnObj);
//
//    if (!tblAtt[sUpstreamLink].fValid())
//        ColumnNotFoundError(tblAtt->fnObj, sUpstreamLink);
//
//    if (!tblAtt[sDownstreamLink].fValid())
//        ColumnNotFoundError(tblAtt->fnObj, sDownstreamLink);
//
//    if (!tblAtt[sDownstreamCoord].fValid())
//        ColumnNotFoundError(tblAtt->fnObj, String("TostreamCoord")); //sDownstreamCoord );
//}
//

MapCatchmentExtraction::MapCatchmentExtraction()
{
}


MapCatchmentExtraction::MapCatchmentExtraction(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}

bool MapCatchmentExtraction::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    bool resource =  executeCatchmentExtraction();


   /* executeFillSink();

    bool resource = true;
    if (resource && ctx != 0) {
        QVariant value;
        value.setValue<IRasterCoverage>(_outRaster);
        ctx->setOutput(symTable, value, _outRaster->name(), itRASTER, _outRaster->resource());
    }*/

	QVariant value;
	value.setValue<IFeatureCoverage>(_outputfeatures);
	logOperation(_outputfeatures, _expression, { _inDrngOrderRaster });
	ctx->addOutput(symTable, value, _outputfeatures->name(), itFEATURE, _outputfeatures->resource());
	_outputfeatures->setAttributes(_outputTable);

	return resource;
}

bool MapCatchmentExtraction::executeCatchmentExtraction()
{
    bool ret(true);

    quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
    _iterEmptyRaster = OperationHelperRaster::initialize(_inDrngOrderRaster.as<IlwisObject>(), itRASTER, copylist);
    _flagRaster = OperationHelperRaster::initialize(_inDrngOrderRaster.as<IlwisObject>(), itRASTER, copylist);

    PixelIterator iterFlag = PixelIterator(_flagRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterDEM = PixelIterator(_inDrngOrderRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);


    PixelIterator iterOut = PixelIterator(_outCatchmentRaster, BoundingBox(), PixelIterator::fXYZ);
    PixelIterator inEnd = iterDEM.end();

    while (iterPos != inEnd)
    {
        Pixel pxl = iterPos.position();
        pxl.z = 0;
       
        if (IsEdgeCell(pxl) || abs(*iterFld(pxl)) == iUNDEF)
            *iterFld(pxl) = 0;
        *iterFlag[pxl] = iUNDEF;

        iterPos++;
    }

    iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

    GetAttributes();

	while (m_vRecords.size() != 0)
	{
		for (unsigned long index = 0; index < m_vRecords.size(); ++index)
		{
			AttCols attFields(m_vRecords[index]);

			//should start with a no upstream channel 
			//or with a channel that upstream(s) have been processed.  
			if (attFields.UpstreamLink.size() == 1 && attFields.UpstreamLink[0] == 0)
			{
				//Extract catchment, and flag the cells with DrainageID that flow to Downstream Coord 
				DelineateCatchment(attFields.DownstreamCoord, attFields.DrainageID);

				//Remove the upstreamlink for the downstream that it drains to	
				UpdateUpstreamLinkID(attFields.DownstreamLink, attFields.DrainageID);

				EraseDrainage(attFields.DrainageID);
			}
		}
	}

	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
	iterFlag = PixelIterator(_flagRaster, BoundingBox(), PixelIterator::fXYZ);
	iterOut = PixelIterator(_outCatchmentRaster, BoundingBox(), PixelIterator::fXYZ);

    while (iterPos != inEnd)
    {
        *iterOut = *iterFlag;
        *iterFlag++;
        *iterOut++;
    }

	m_vRecords.resize(0);
	SetAttributeTable();

    return ret;
}


Ilwis::OperationImplementation* MapCatchmentExtraction::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
    return new MapCatchmentExtraction(metaid, expr);
}

Ilwis::OperationImplementation::State MapCatchmentExtraction::prepare(ExecutionContext* ctx, const SymbolTable& st)
{
    OperationImplementation::prepare(ctx, st);
    QString drnetwkraster = _expression.parm(0).value();
	QString outputName = _expression.parm(0, false).value();

    QString flowraster = _expression.parm(1).value();
    QString outcatchment = _expression.parm(2).value();

    IRasterCoverage _inDngOrderRaster;
    IRasterCoverage _inFldRaster;

    IRasterCoverage _outCatchmentRaster;
    IRasterCoverage _iterEmptyRaster;

    if (!_inDrngOrderRaster.prepare(drnetwkraster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, drnetwkraster, "");
        return sPREPAREFAILED;
    }

    if (!_inFldRaster.prepare(flowraster, itRASTER)) {
        ERROR2(ERR_COULD_NOT_LOAD_2, flowraster, "");
        return sPREPAREFAILED;
    }

    int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
    _outCatchmentRaster = OperationHelperRaster::initialize(_inDngOrderRaster.as<IlwisObject>(), itRASTER, copylist);
    if (!_outCatchmentRaster.isValid()) {
        ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
        return sPREPAREFAILED;
    }

    _xsize = _inDrngOrderRaster->size().xsize();
    _ysize = _inDrngOrderRaster->size().ysize();

    IDomain dom("code=domain:value");
    _outCatchmentRaster->datadefRef() = DataDefinition(dom);

    for (quint32 i = 0; i < _outCatchmentRaster->size().zsize(); ++i) {
        QString index = _outCatchmentRaster->stackDefinition().index(i);
        _outCatchmentRaster->setBandDefinition(index, DataDefinition(dom));
    }

	_outputfeatures.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outputName));

	_inputgrf = _inDngOrderRaster->georeference();
	_csy = _inputgrf->coordinateSystem();
	_outputfeatures->coordinateSystem(_csy);
	Envelope env = _inDngOrderRaster->georeference()->envelope();
	_outputfeatures->envelope(env);

    return sPREPARED;
}

quint64 MapCatchmentExtraction::createMetadata()
{
    OperationResource operation({ "ilwis://operations/MapCatchmentExtraction" });
    operation.setSyntax("MapCatchmentExtraction(DrainageNetworkOrderMap,FlowDiractionMap)");
    operation.setDescription(TR("Extract catchment based on DrainageNetworkOrder Map and FlowDiraction Map"));
    operation.setInParameterCount({ 2 });
    operation.addInParameter(0, itRASTER, TR("Drainage Net Work Ordering Map"), TR("input raster DEM with numeric domain"));
    operation.addInParameter(1, itRASTER, TR("Flow Direction Map"), TR("input raster DEM with numeric domain"));
    operation.parameterNeedsQuotes(1);

	operation.setOutParameterCount({ 3 });

	operation.addOutParameter(0, itTABLE, TR("output table"), TR("output table with the results of the catchment extraction"));
	operation.addOutParameter(1, itPOLYGON, TR("output polygon"), TR("output polygon with the results of the catchment extraction"));
	operation.addOutParameter(2, itRASTER, TR("output raster"), TR("output raster with the results of the catchment extraction"));
	operation.setKeywords("catchment, extraction, raster,table, polygon");

    mastercatalog()->addItems({ operation });
    return operation.id();
}



void MapCatchmentExtraction::SetAttributeTable()
{
	// prepare table

	IFlatTable newTable;
	newTable.prepare();

	//newTable->addColumn("StreamID", _orderDomain);
	/*newTable->addColumn("UpstreamLinkID", IlwisObject::create<IDomain>("text"), true);
	newTable->addColumn("UpstreamCoord", IlwisObject::create<IDomain>("text"));
	newTable->addColumn("UpstreamElevation", IlwisObject::create<IDomain>("value"));
	newTable->addColumn("DownstreamLinkID", IlwisObject::create<IDomain>("value"));*/

	
	_outputTable = newTable;

}


bool MapCatchmentExtraction::IsEdgeCell(Pixel pxl)
{
    if (pxl.y == 0 || pxl.y == _ysize - 1 ||
        pxl.x == 0 || pxl.x == _xsize - 1)
        return true;
    else
        return false;
}


bool MapCatchmentExtraction::fLatLonCoords()
{
	/*CoordSystemLatLon* csll = mp->cs()->pcsLatLon();
	return (0 != csll);*/
	return false;
}

//void MapCatchmentExtraction::CompitableGeorefs(FileName fn, Map mp1, Map mp2)
//{
//	bool fIncompGeoRef = false;
//	if (mp1->gr()->fGeoRefNone() && mp2->gr()->fGeoRefNone())
//		fIncompGeoRef = mp1->rcSize() != mp2->rcSize();
//	else
//		fIncompGeoRef = mp1->gr() != mp2->gr();
//	if (fIncompGeoRef)
//		throw ErrorIncompatibleGeorefs(mp1->gr()->sName(true, fn.sPath()),
//			mp2->gr()->sName(true, fn.sPath()), fn, errMapCatchmentExtraction);
//
//}

void MapCatchmentExtraction::GetAttributes()
{
	//Retrieve the network link attributes needed to be able 
	//to extract the catchment area

	ITable tblAtt = _inDrngOrderRaster->attributeTable();
	std::vector<QVariant> colUpstreamLink = tblAtt->column(sUpstreamLink);
	std::vector<QVariant> colDownstreamLink = tblAtt->column(sDownstreamLink);
	std::vector<QVariant> colDownstreamCoord = tblAtt->column(QString("TostreamCoord"));

	std::vector<QVariant> colPrimaryKey = tblAtt->column(_inDrngOrderRaster->primaryKey());

	long iSize = tblAtt->recordCount();
	AttCols ac;
	for (long i = 1; i <= iSize; i++) 
	{
		QVariant val = colPrimaryKey[i];
		ac.DrainageID = val.toInt();
		if (ac.DrainageID == iUNDEF)
			continue;
		ac.DownstreamLink = colDownstreamLink[i].toInt();
		if (ac.DownstreamLink == 0)
			continue;
		QString coordsStr = colDownstreamCoord[i].toString();

		if (coordsStr.isEmpty())
			continue;

		coordsStr.replace("(", " ");
		coordsStr.replace(")", " ");

		QStringList coodrs = coordsStr.split(",");

		QString xstr = coodrs[0];
		QString ystr = coodrs[1];

		Coordinate crd;
		crd.x = xstr.toDouble();
		crd.y = ystr.toDouble();
		crd.z = 0;
		ac.DownstreamCoord = _inDrngOrderRaster->georeference()->coord2Pixel(crd);

		SplitUpStreamLink(colUpstreamLink[i].toString(), ac.UpstreamLink);
		m_vRecords.push_back(ac);
	}

}


void MapCatchmentExtraction::SplitUpStreamLink(QString s, std::vector<long>& results)
{
	s.replace("(", " ");
	s.replace(")", " ");
	QStringList strlst = s.split(",");

	results.clear();
	for (unsigned int i = 0; i < strlst.size(); i++)
	{
		long res = strlst[i].toInt();
		if (res != iUNDEF)
			results.push_back(res);
	}
}

void MapCatchmentExtraction::ComputeCatchmentAttributes()
{
	//trq.SetText(TR("Create merged catchment attribute table"));

	////Retrieve attributes from drainage network attribute table needed 
	////for updating the catchment attributes  
	//Table tblAtt = mp->tblAtt();
	//Column colUpstreamLink = tblAtt->col(sUpstreamLink);
	//Column colDownstreamLink = tblAtt->col(sDownstreamLink);
	//Column colFlowLength = tblAtt->col(String("Length"));

	//DomainSort* pdsrt = colUpstreamLink->dmKey()->pdsrt();
	//if (!pdsrt)
	//	throw ErrorObject(TR("Source map must have domain class or id"));
	//long iSize = pdsrt->iSize();

	////Define catchment attributes
	//Column cDrainageID = m_tbl->colNew(String("DrainageID"), Domain("value"), ValueRange(1, 32767, 1));
	//cDrainageID->SetOwnedByTable(true);
	//cDrainageID->SetReadOnly(true);
	//Column cUpstreamLinkCatchment = m_tbl->colNew(String("UpstreamLinkCatchment"), Domain("string"));
	//cUpstreamLinkCatchment->SetOwnedByTable(true);
	//cUpstreamLinkCatchment->SetReadOnly(true);
	//Column cDownstreamLinkCatchment = m_tbl->colNew(String("DownstreamLinkCatchment"), Domain("value"), ValueRange(1, 32767, 1));
	//cDownstreamLinkCatchment->SetOwnedByTable(true);
	//cDownstreamLinkCatchment->SetReadOnly(true);
	//Column cPerimeter = m_tbl->colNew(String("Perimeter"), Domain("value"), ValueRange(1, 1.0e300, 0.01));
	//Column cCatchmentArea = m_tbl->colNew(String("CatchmentArea"), Domain("value"), ValueRange(1, 1.0e300, 0.01));
	//Column cTotalUpstreamArea = m_tbl->colNew(String("TotalUpstreamArea"), Domain("value"), ValueRange(1, 1.0e300, 0.01));
	//Column cLongestFlowLength = m_tbl->colNew(String("LongestFlowLength"), Domain("value"), ValueRange(1, 1.0e300, 0.01));

	////Retrieve upstream link attributes needed to compute the total area
	//AttUpstreamLink vUpstreamLinks;
	//for (long i = 1; i <= iSize; i++)
	//{
	//	long iDrainageID = pdsrt->iOrd(i);
	//	vUpstreamLinks.DrainageID = iDrainageID;
	//	SplitString(colUpstreamLink->sValue(i), vUpstreamLinks.UpstreamLink);
	//	m_vvUpstreamLinks.push_back(vUpstreamLinks);
	//}

	//long index = 0;
	//for (long i = 1; i <= iSize; i++)
	//{
	//	long iDrainageID = pdsrt->iOrd(i);
	//	if (iDrainageID == iUNDEF)
	//		continue;

	//	cDrainageID->PutVal(iDrainageID, iDrainageID);
	//	cUpstreamLinkCatchment->PutVal(iDrainageID, colUpstreamLink->sValue(i));
	//	cDownstreamLinkCatchment->PutVal(iDrainageID, colDownstreamLink->iValue(i));
	//	cPerimeter->PutVal(iDrainageID, m_cPerimeter->rValue(i));
	//	cCatchmentArea->PutVal(iDrainageID, m_cArea->rValue(i));

	//	//Initialize the totalarea  to the catchment area itsself, if it is a source link, it should be
	//	//Otherwise, assign an no data value, this will be computed later
	//	AttUpstreamLink vULs(m_vvUpstreamLinks[i - 1]);
	//	if (vULs.UpstreamLink.size() == 1 && vULs.UpstreamLink[0] == 0)
	//		cTotalUpstreamArea->PutVal(iDrainageID, m_cArea->rValue(i));
	//	else
	//		cTotalUpstreamArea->PutVal(iDrainageID, rUNDEF);

	//	cLongestFlowLength->PutVal(iDrainageID, colFlowLength->rValue(i));

	//	if (trq.fUpdate(index, (int)m_vRecords.size())) return;
	//	index++;
	//}
	//trq.fUpdate((int)m_vRecords.size(), (int)m_vRecords.size());

	//ComputeTotalUpstreamArea(pdsrt, cCatchmentArea, cTotalUpstreamArea);
	//m_vvUpstreamLinks.resize(0);
}

//void MapCatchmentExtraction::ComputeTotalUpstreamArea(DomainSort* pdsrt, Column cArea, Column cTotalArea)
//{
	//bool fComputeTotalArea = true;
	//long iSize = pdsrt->iSize();
	//trq.SetText(TR("Calculate the total upstream catchment area"));
	//while (fComputeTotalArea)
	//{
	//	fComputeTotalArea = false;
	//	for (long i = 1; i <= iSize; i++)
	//	{
	//		long iDrainageID = pdsrt->iOrd(i);
	//		if (iDrainageID == iUNDEF)
	//			continue;

	//		if (cTotalArea->rValue(i) == rUNDEF)
	//		{
	//			fComputeTotalArea = true;
	//			AttUpstreamLink vULs(m_vvUpstreamLinks[i - 1]);
	//			vector<long> vLinks = vULs.UpstreamLink;
	//			double rArea = 0;
	//			for (vector<long>::iterator pos = vLinks.begin();
	//				pos < vLinks.end(); ++pos)
	//			{
	//				//search ID in domain, return index, if the ID is found 
	//				String sLbl("%li", (*pos));
	//				long iRaw = pdsrt->iOrd(sLbl);
	//				if (cTotalArea->rValue(iRaw) == rUNDEF)
	//					break;
	//				rArea += cTotalArea->rValue(iRaw);
	//				cTotalArea->PutVal(iDrainageID, rArea);
	//			}
	//		}
	//		if (trq.fUpdate(i, iSize)) return;
	//	}
	//} //while()
//}

//void MapCatchmentExtraction::ComputerCenterPolygon(FileName fn)
//{
	////First lable point map of the polygon
	////then, put the point coordinates to the catchment attribute table
	//trq.SetText(TR("Compute the center of polygon"));
	//FileName fnTmpPoint(fnObj, ".mpp");
	//fnTmpPoint = FileName::fnUnique(fnTmpPoint);
	//String sExpr("PointMapPolLabels(%S)", fn.sFullPathQuoted(false));
	//PointMap ptTmpMap;
	//ptTmpMap = PointMap(fnTmpPoint, sExpr);
	//ptTmpMap->Calc();

	//Domain dmcrd;
	//dmcrd.SetPointer(new DomainCoord(ptTmpMap->cs()->fnObj));
	//Column cCenterCatchment = m_tbl->colNew(String("CenterCatchment"), dmcrd);

	//Coord crd;
	//long iPoint = ptTmpMap->iFeatures();
	//for (long i = 0; i < iPoint; ++i) {
	//	long iRaw = ptTmpMap->iRaw(i);
	//	if (iRaw == iUNDEF)
	//		continue;
	//	Coord crd = ptTmpMap->cValue(i);
	//	cCenterCatchment->PutVal(iRaw, crd);
	//	if (trq.fUpdate(i, iPoint)) return;
	//}
	//ptTmpMap->fErase = true;
//}

double MapCatchmentExtraction::GetDistance(Pixel& rc)
{
	double dist(0);
	//Return a cell that the current given cell rc flows to,
	//otherwise, return the same cell as the given cell, This means
	//that the given cell doesn't flow to any other cell.  

	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);

	Pixel pospxl;
	pospxl.x = rc.x - 1;
	pospxl.y = rc.y - 1;
	pospxl.z = 0;

	int iPos = *iterFld[pospxl];
	Pixel rc2;
	rc2.z = 0;

	switch (iPos)
	{
	case 1: 	//East
		rc2.y = rc.y;
		rc2.x = rc.x + 1;
		break;
	case 2:  //South East 
		rc2.y = rc.y + 1;
		rc2.x = rc.x + 1;
		break;
	case 3: 	//South
		rc2.y = rc.y + 1;
		rc2.x = rc.x;
		break;
	case 4: //South West
		rc2.y = rc.y + 1;
		rc2.x = rc.x - 1;
		break;
	case 5:	//West
		rc2.y = rc.y;
		rc2.x = rc.x - 1;
		break;
	case 6:	//North West 
		rc2.y = rc.y - 1;
		rc2.x = rc.x - 1;
		break;
	case 7:	//North
		rc2.y = rc.y - 1;
		rc2.x = rc.x;
		break;
	case 8:	//North East
		rc2.y = rc.y - 1;
		rc2.x = rc.x + 1;
		break;
	default:
		rc2.y = rc.y;
		rc2.x = rc.x;
		break;
	}
	Coordinate c1 = _inDrngOrderRaster->georeference()->pixel2Coord(rc);
	Coordinate c2 = _inDrngOrderRaster->georeference()->pixel2Coord(rc2);

	dist = rDistance(c1, c2);
	rc.y = rc2.y;
	rc.x = rc2.x;
	return dist;
}

double MapCatchmentExtraction::rDistance(Coordinate cd1, Coordinate cd2)
{
	double rDist;
	if (fLatLonCoords())
	{
		double rRadi = rDefaultEarthRadius;
		IConventionalCoordinateSystem projectedCsy = _inDrngOrderRaster->coordinateSystem().as<ConventionalCoordinateSystem>();
		if (projectedCsy->isValid())
			rRadi = projectedCsy->ellipsoid()->majorAxis();

		LatLon llStart = LatLon(cd1.y, cd1.x);
		LatLon llEnd = LatLon(cd2.y, cd2.x);
		if (projectedCsy->ellipsoid()->isSpherical())
		{
			if (llStart.Lat() == llEnd.Lat() && llStart.Lon() == llEnd.Lon())
				rDist = 0; //seems a bug in rEllipsoidalDistance, always get some value, even when llStart and llEnd the same? 
			else
				rDist = projectedCsy->ellipsoid()->distance(llStart, llEnd);
			if (rDist < 8000)
				return rDist;
		}

		rDist = rSphericalDistance(rRadi, llStart, llEnd);
	}
	else
	{
		double dx = (cd1.x - cd2.x);
		double dy = (cd1.y - cd2.y);
		rDist = sqrt(dx * dx + dy * dy);
	}
	return rDist;
}

void MapCatchmentExtraction::ComputeCenterDrainage()
{
	//Retrieve UpstreamCoord attributes from drainage network attribute table needed
  //to be able to trace the flow path and get the center of drainage.
	ITable tblAtt = _inDrngOrderRaster->attributeTable();
	std::vector<QVariant> colUpstreamCoord = tblAtt->column(QString("UpstreamCoord"));

	std::vector<QVariant> colUpstreamLink = tblAtt->column(sUpstreamLink);
	std::vector<QVariant> colLength = tblAtt->column(QString("Length"));

	std::vector<QVariant> colPrimaryKey = tblAtt->column(_inDrngOrderRaster->primaryKey());

	/*Column colUpstreamCoord = tblAtt->col(String("UpstreamCoord"));
	Column colLength = tblAtt->col(String("Length"));

	Domain dmcrd;
	dmcrd.SetPointer(new DomainCoord(mp->cs()->fnObj));
	Column cCenterDrainage = m_tbl->colNew(String("CenterDrainage"), dmcrd);

	DomainSort* pdsrt = colUpstreamCoord->dmKey()->pdsrt();
	if (!pdsrt)
		throw ErrorObject(TR("Source map must have domain class or id"));
	long iSize = pdsrt->iSize();
	for (long i = 1; i <= iSize; i++)
	{
		long iDrainageID = pdsrt->iOrd(i);
		if (iDrainageID == iUNDEF)
			continue;

		RowCol rc = mp->gr()->rcConv(colUpstreamCoord->cValue(i));
		double rLength = colLength->rValue(i) / 2;
		double rDistance = 0;
		while (rDistance < rLength)
		{
			double rDist = GetDistance(rc);
			if (rDist == 0)
				break;
			rDistance = rDistance + rDist;
		}
		rc.Row -= 1;
		rc.Col -= 1;
		Coord crd = mp->gr()->cConv(rc);
		cCenterDrainage->PutVal(iDrainageID, crd);
		if (trq.fUpdate(i, iSize)) return;
	}*/
}

long MapCatchmentExtraction::DelineateCatchment(Pixel pxl, long iFlag)
{
	/*For the specified downstream cell in loaction rc, 
	check whether its neighboring cells flow to it,
	If true, flag the cells with iFlag in m_vDrainage, then call the function recursively   
	The recursion stops when it reaches a cell that has no flow to it 
	location number
		-------
		|6|7|8|
		-------
		|5| |1|
		-------
		|4|3|2|
		-------*/
	 
	long iFlow = 1;
	bool isFlow; //determine if the neighboring cell flows to the cell in location rc 
	long in, jn;
	Pixel pospxl;
	pospxl.z = 0;

	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterFlag = PixelIterator(_flagRaster, BoundingBox(), PixelIterator::fXYZ);


	for (int iNr = 1; iNr < 9; iNr++)
	{
		isFlow = false;
		switch (iNr)
		{
		case 1: {	//East
			if (pxl.x != _xsize - 1)
			{
				pospxl.y = pxl.y;
				pospxl.x = pxl.x + 1;
				isFlow = (*iterFld[pospxl] == 5 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 2: { //South East 
			if (pxl.x != _xsize - 1 && pxl.y != _ysize - 1)
			{
				pospxl.y = pxl.y + 1;
				pospxl.x = pxl.x + 1;
				isFlow = (*iterFld[pospxl] == 6 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 3: {	//South
			if (pxl.y != _ysize - 1)
			{
				pospxl.y = pxl.y + 1;
				pospxl.x = pxl.x;
				isFlow = (*iterFld[pospxl] == 6 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 4: { //South West
			if ( pxl.x != 0 && pxl.y != _ysize - 1)
			{
				pospxl.y = pxl.y + 1;
				pospxl.x = pxl.x - 1;
				isFlow = (*iterFld[pospxl] == 8 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 5: {	//West
			if (pxl.x != 0)
			{
				pospxl.y = pxl.y;
				pospxl.x = pxl.x - 1;
				isFlow = (*iterFld[pospxl] == 1 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 6: {	//North West 
			if (pxl.x != 0 && pxl.y != 0)
			{
				isFlow = false;
				pospxl.y = pxl.y - 1;
				pospxl.x = pxl.x - 1;
				isFlow = (*iterFld[pospxl] == 2 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 7: {	//North
			if (pxl.y != 0)
			{
				pospxl.y = pxl.y - 1;
				pospxl.x = pxl.x;
				isFlow = (*iterFld[pospxl] == 3 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		case 8: {	//North East
			if (pxl.x != _xsize - 1 && pxl.y != 0)
			{
				pospxl.y = pxl.y - 1;
				pospxl.x = pxl.x + 1;
				isFlow = (*iterFld[pospxl] == 4 && *iterFlag[pospxl] == iUNDEF);
			}
		}
			  break;
		}
		if (isFlow)
		{
			pospxl.y = pxl.y;
			pospxl.x = pxl.x;
			iFlow += DelineateCatchment(pxl, iFlag);

		}
		*iterFlag[pxl] = iFlag;
		//m_vDrainageMap[iRow][iCol] = iFlag;
	}
	return iFlow;
}

long MapCatchmentExtraction::FindDownstreamIndex(long DowmstreamID)
{

	for (vector<AttCols>::iterator pos = m_vRecords.begin(); pos < m_vRecords.end(); ++pos)
	{
		AttCols ac(*pos);
		if (ac.DrainageID == DowmstreamID)
			return (long)(pos - m_vRecords.begin());
	}
	return -1;
}

void MapCatchmentExtraction::UpdateUpstreamLinkID(long DrainageID, long UpstreamID)
{

	if (DrainageID != iUNDEF)
	{
		//Find the downstream index in m_vRecords
		//returns position for the downstream in m_vRecords
		long iIndex = FindDownstreamIndex(DrainageID);

		if (iIndex > 0)
		{
			AttCols attFields(m_vRecords[iIndex]);

			if ((attFields.UpstreamLink.size() == 1) && (attFields.UpstreamLink[0] == UpstreamID))
				m_vRecords[iIndex].UpstreamLink[0] = 0;
			else if (attFields.UpstreamLink.size() > 1)
			{
				vector<long>::iterator pos;
				pos = find(attFields.UpstreamLink.begin(), attFields.UpstreamLink.end(), UpstreamID);
				if (pos != attFields.UpstreamLink.end())
				{
					attFields.UpstreamLink.erase(pos);
					m_vRecords[iIndex].UpstreamLink = attFields.UpstreamLink;
				}
			}
		}
	}
}

void MapCatchmentExtraction::EraseDrainage(long DrainageID)
{
	for (vector<AttCols>::iterator pos = m_vRecords.begin(); pos < m_vRecords.end(); ++pos)
	{
		AttCols ac(*pos);
		if (ac.DrainageID == DrainageID)
		{
			m_vRecords.erase(pos);
			break;
		}
	}
}


bool MapCatchmentExtraction::fEllipsoidalCoords()
{
	//CoordSystemViaLatLon* csviall = mp->cs()->pcsViaLatLon();
	//bool fSpheric = true;
	//if (csviall)
	//	fSpheric = (csviall->ell.fSpherical());
	//return (0 != csviall && 0 == fSpheric);
	return false;
}
