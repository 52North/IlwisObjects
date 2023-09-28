
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
//#include "geos/geom/PrecisionModel.h"

#include "symboltable.h"
#include "columndefinition.h"
#include "basetable.h"
#include "flattable.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "ilwisoperation.h"
#include "ilwiscoordinate.h"
#include "blockiterator.h"
#include "pixeliterator.h"
#include "featurecoverage.h"
#include "table.h"
#include "feature.h"
#include "featureiterator.h"
#include "conventionalcoordinatesystem.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/CoordinateSequenceFactory.h"
#include "geos/geom/GeometryFactory.h"
#include "geos/geom/linestring.h"
#include "ellipsoid.h"
#include "itemdomain.h"
#include "thematicitem.h"

#include "MapDrainageNetworkOrdering.h"

using namespace Ilwis;
using namespace Hydroflow;

REGISTER_OPERATION(DrainageNetworkOrdering)

const double rDefaultEarthRadius = 6371007.0;

static double rSphericalDistance(double rRadius, const LatLon& ll_1, const LatLon& ll_2)
{
	if (ll_1.Lat()== rUNDEF || ll_1.Lon() == rUNDEF || ll_2.Lat()==rUNDEF || ll_2.Lon() == rUNDEF)
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

DrainageNetworkOrdering::DrainageNetworkOrdering()
{
}


DrainageNetworkOrdering::DrainageNetworkOrdering(quint64 metaid, const Ilwis::OperationExpression& expr) : OperationImplementation(metaid, expr)
{

}

bool DrainageNetworkOrdering::execute(ExecutionContext* ctx, SymbolTable& symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	bool resource = executeDrainageNetworkOrdering();

	if (resource && ctx != 0) 
	{
		QVariant value;
		value.setValue<ITable>(_outputTable);
		logOperation(_outputTable, _expression);
		ctx->setOutput(symTable, value, _outputTable->name(), itTABLE, _outputTable->resource());

		if (_outputfeatures.isValid())
		{
			_outputfeatures->setAttributes(_outputTable);
			QVariant value;
			value.setValue<IFeatureCoverage>(_outputfeatures);
			logOperation(_outputfeatures, _expression, { _inRaster });
			ctx->addOutput(symTable, value, _outputfeatures->name(), itFEATURE, _outputfeatures->resource());
		}

		if (_outDrainageRaster.isValid()) {
			_outDrainageRaster->setAttributes(_outputTable);
			QVariant outraster;
			outraster.setValue<IRasterCoverage>(_outDrainageRaster);
			logOperation(_outDrainageRaster, _expression, { _inRaster, _inFldRaster, _inDrainageRaster });
			ctx->addOutput(symTable, outraster, _outDrainageRaster->name(), itRASTER, _outDrainageRaster->resource());
		}
	}

	return resource;
}

Ilwis::OperationImplementation* DrainageNetworkOrdering::create(quint64 metaid, const Ilwis::OperationExpression& expr)
{
	return new DrainageNetworkOrdering(metaid, expr);
}

Ilwis::OperationImplementation::State DrainageNetworkOrdering::prepare(ExecutionContext* ctx, const SymbolTable& st)
{

	OperationImplementation::prepare(ctx, st);
	QString inraster = _expression.parm(0).value();
	QString outputName = _expression.parm(0, false).value();
	QString flowraster = _expression.parm(1).value();
	QString drnetwkraster = _expression.parm(2).value();
	m_rMinLen = _expression.parm(3).value().toInt();

	if (!_inRaster.prepare(inraster, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, inraster, "");
		return sPREPAREFAILED;
	}

	if (!_inFldRaster.prepare(flowraster, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, flowraster, "");
		return sPREPAREFAILED;
	}

	_inFldRaster.set(_inFldRaster->clone());

	if (!_inDrainageRaster.prepare(drnetwkraster, itRASTER)) {
		ERROR2(ERR_COULD_NOT_LOAD_2, drnetwkraster, "");
		return sPREPAREFAILED;
	}

	_inDrainageRaster.set(_inDrainageRaster->clone());

	int copylist = itRASTERSIZE | itENVELOPE | itCOORDSYSTEM | itGEOREF;
	_outDrainageRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);
	if (!_outDrainageRaster.isValid()) {
		ERROR1(ERR_NO_INITIALIZED_1, "output rastercoverage");
		return sPREPAREFAILED;
	}

	_xsize = _inRaster->size().xsize();
	_ysize = _inRaster->size().ysize();

	// prepare feature
	_inputgrf = _inRaster->georeference();
	_outputfeatures.prepare(QString(INTERNAL_CATALOG + "/%1").arg(outputName));
	_csy = _inputgrf->coordinateSystem();
	_outputfeatures->coordinateSystem(_csy);
	Envelope env = _inRaster->georeference()->envelope();
	_outputfeatures->envelope(env);

	if (_outDrainageRaster.isValid())
	{
		QString orderName = "OrderID";
		_orderDomain.prepare();
		//_orderDomain->name(orderName);
		_idrange = new NamedIdentifierRange();
		_orderDomain->range(_idrange);

		DataDefinition def(_orderDomain);
		_outDrainageRaster->datadefRef() = def;
		for (int band = 0; band < _outDrainageRaster->size().zsize(); ++band) {
			_outDrainageRaster->datadefRef(band) = def;
		}

		return sPREPARED;
	}

	return sPREPAREFAILED;
}


quint64 DrainageNetworkOrdering::createMetadata()
{
	OperationResource operation({ "ilwis://operations/DrainageNetworkOrdering" });
	operation.setSyntax("DrainageNetworkOrdering(DEMmap,FlowDiractionmap,DrainageNetworkmap,MinimumDrainageLength)");
	operation.setDescription(TR("Finds individual streams within a drainage network and assigns a unique ID to each stream. The operation delivers an output raster map, an output segment map and an output attribute table."));
	operation.setInParameterCount({ 4 });
	operation.addInParameter(0, itRASTER, TR("input raster"), TR("input DEM map with numeric domain"));
	operation.addInParameter(1, itRASTER, TR("input raster"), TR("input flow direction map"));
	operation.addInParameter(2, itRASTER, TR("input raster"), TR("input drainage network map that is the output of a previous Drainage network extraction operation"));
	operation.addInParameter(3, itINT32, TR("input value"), TR("input minimum drainage length (m) that a stream should have to remain in the drainage network."));

	operation.parameterNeedsQuotes(1);
	operation.setOutParameterCount({ 3 });

	operation.addOutParameter(0, itTABLE, TR("output table"), TR("output table with the results of the drainage network ordering"));
	operation.addOutParameter(1, itLINE, TR("output segment"), TR("output segment with the results of the drainage network ordering"));
	operation.addOutParameter(2, itRASTER, TR("output raster"), TR("output raster with the results of the drainage network ordering"));
	operation.setKeywords("drainage, network, ordering");

	operation.checkAlternateDefinition();
	mastercatalog()->addItems({ operation });
	return operation.id();
}


bool DrainageNetworkOrdering::executeDrainageNetworkOrdering()
{
	IniParms();

	quint64 copylist = itRASTERSIZE | itENVELOPE | itINTEGER;
	_iterEmptyRaster = OperationHelperRaster::initialize(_inRaster.as<IlwisObject>(), itRASTER, copylist);

	PixelIterator iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterInRawDrainage = PixelIterator(_inDrainageRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);

	// Compute maxval of drainage (this should be a boolean-like map with 2 values, e.g. False, True or 0, 1, but we allow e.g. 1, 2). Undefs are acceptable.
	NumericStatistics stats;
	stats.calculate(iterInRawDrainage, iterInRawDrainage.end(), NumericStatistics::pBASIC);
	double maxDrainage = stats[NumericStatistics::pMAX];
	iterInRawDrainage = PixelIterator(_inDrainageRaster, BoundingBox(), PixelIterator::fXYZ); // rewind

	// Check if we are dealing with FlowDirection.dom; if yes, then recalc raw vals of flow direction map. We also allow Value maps with values 0 til 8 (0 = flat / undef, 1 til 8 are the directions).
	bool itemdomain = false;
	IDomain dom = _inFldRaster->datadefRef().domain();
	if (dom.isValid() && hasType(dom->valueType(), itTHEMATICITEM | itNUMERICITEM | itTIMEITEM | itNAMEDITEM))
		itemdomain = true;

	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator inEnd = iterDEM.end();

	std::vector<double> vals;
	while (iterInRawDrainage != inEnd)
	{
		double val = *iterInRawDrainage;
		*iterDrainage = (val == maxDrainage) ? 1 : 0;
		*iterInRawDrainage++;
		*iterDrainage++;
	}


	// temp feature coverage
	QString tmpName = "tmpName";
	IFeatureCoverage tmpfeatures;

	tmpfeatures.prepare(QString(INTERNAL_CATALOG + "/%1").arg(tmpName));
	tmpfeatures->coordinateSystem(_inRaster->georeference()->coordinateSystem());
	tmpfeatures->envelope(_inRaster->georeference()->envelope());

	while (iterPos != inEnd)
	{
		Pixel pxl = iterPos.position();
		pxl.z = 0;
		if (*iterDrainage(pxl) != rUNDEF)
		{
			*iterDrainage(pxl) -= 2; //Initialize input drainage map
		}
		if (IsEdgeCell(pxl) || *iterFld(pxl) == rUNDEF )
			*iterFld(pxl) = 0;
		else if (itemdomain)
			++*iterFld(pxl); // convert back to FlowDirection raw values whereby 0=flat, 1..8 are the direction

		iterPos++;
	}

	double rPixDist = _inRaster->georeference()->pixelSize() * 1.41; //max. a pixel distance
	iterPos = PixelIterator(_iterEmptyRaster, BoundingBox(), PixelIterator::fXYZ);

	while (iterPos != inEnd)
	{
		Pixel pxl = iterPos.position();
		pxl.z = 0;
		if (IsEdgeCell(pxl))
		{
			iterPos++;
			continue;
		}

		if (IsSourceCell(pxl))
		{
			Pixel pospxl = pxl;
			pospxl.z = 0;
			m_rLength = 0;
			m_vStreamLink.resize(0);
			int iLink = IdentifyLink(pxl);

			if ((m_rLength - rPixDist) <= m_rMinLen) //remove drainage shorter than threshold,also remove by default a drainage of only one pixel
			{
				FlagDrainageMap(iOffDrainage, iLink);
				if ((iLink == iJunction) && IsJunctionExists(pxl))
				{
					if (IsJunction(pxl) != true)
					{
						long iIndex = GetUpstream(pxl);
						EraseJunction(pxl);
						m_rLength = m_vRecords[iIndex].Length;
						m_vStreamLink.resize(0);
						iLink = IdentifyLink(pxl);
						
						PatchSegment(tmpfeatures, m_vRecords[iIndex].id);
						
						m_vRecords[iIndex].DownstreamCoord = m_vStreamLink[m_vStreamLink.size() - 1];
						m_vRecords[iIndex].TostreamCoord = m_vStreamLink[m_vStreamLink.size() - 1];
						if (iLink == iJunction)
						{
							StoreJunction(pxl);
							if (m_vStreamLink.size() > 1)
								m_vRecords[iIndex].TostreamCoord = m_vStreamLink[m_vStreamLink.size() - 2];
						}
						FlagDrainageMap(m_vRecords[iIndex].id, iLink);
						m_vRecords[iIndex].Length = m_rLength;
					}
				}
			}
			else
			{
				m_iLinkNumber++;
				AddDomainItem(m_iLinkNumber);
				StoreSegment(tmpfeatures, m_iLinkNumber, false);
				UpdateRecord();
				if (iLink == iJunction)
				{
					StoreJunction(pxl);
					if (m_vStreamLink.size() > 1)
						m_rec.TostreamCoord = m_vStreamLink[m_vStreamLink.size() - 2];
				}
				FlagDrainageMap(m_iLinkNumber, iLink);
				m_vRecords.push_back(m_rec);
			}
		} //IsSourceCell
		iterPos++;
	}
	CleanSegment(_outputfeatures, tmpfeatures);

	while (m_vJunction.size() != 0)
	{
		for (long index = 0; index < (long)m_vJunction.size(); ++index)
		{
			Pixel node = m_vJunction[index];
			m_vStreamLink.resize(0);
			std::vector<long> vUpstreamLink;

			if (IsAllUpstreamIdentified(node, vUpstreamLink))
			{
				m_vStreamLink.push_back(node);
				Pixel rcDownCell = node;
				int iFlow = GetDownStreamCell(rcDownCell);
				if (iFlow != iInValidFlow)
				{
					m_iLinkNumber++;

					Coordinate c1 = _inRaster->georeference()->pixel2Coord(node);;
					Coordinate c2 = _inRaster->georeference()->pixel2Coord(rcDownCell);

					m_rLength = rDistance(c1, c2);
					int iLink = IdentifyLink(rcDownCell);
					AddDomainItem(m_iLinkNumber);
					StoreSegment(_outputfeatures, m_iLinkNumber, true);
					m_iStrahler = ComputeStrahlerOrder(vUpstreamLink);
					m_iStreve = ComputeStreveOrder(vUpstreamLink);
					UpdateRecord();
					if (iLink == iJunction) {
						StoreJunction(rcDownCell);
						if (m_vStreamLink.size() > 1)
							m_rec.TostreamCoord = m_vStreamLink[m_vStreamLink.size() - 2];
					}
					FlagDrainageMap(m_iLinkNumber, iLink);
					DownstreamLink(vUpstreamLink);
					m_vRecords.push_back(m_rec);
				}
				EraseJunction(node);
				//if (trq.fUpdate(index, (long)m_vJunction.size())) return false;
			}
			else
				EraseJunction(node);
		} //for

	}

	iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);

	FillTableRecords();
	return true;

}


void DrainageNetworkOrdering::PatchSegment(IFeatureCoverage sm, long iSegID)
{
	long flg(0);
	for (auto feature : sm)
	{
		geos::geom::LineString* ls_geom =
			dynamic_cast<geos::geom::LineString*>(feature->geometry().get());

		if (ls_geom && ls_geom->isValid() && !tmpFeatureFlags[flg])
		{
			if (ls_geom->getSRID() == iSegID)
			{
				geos::geom::CoordinateSequence* crdbuf1 = ls_geom->getCoordinates();
	
				geos::geom::CoordinateSequence* crdbufnew = crdbuf1->clone();

				geos::geom::Geometry* geometry = sm->geomfactory()->createLineString(crdbufnew);
				geometry->setSRID(iSegID);

				for (int i = 0; i < m_vStreamLink.size(); ++i)
				{
					Coordinate c;
					Pixel pxl = m_vStreamLink[i];
					c = _inRaster->georeference()->pixel2Coord(pxl);
					crdbufnew->add(c);
				}

				tmpFeatureFlags[flg] = true;

				if (geometry->isValid())
				{
					sm->newFeature(geometry);
					tmpFeatureFlags.push_back(false);
				}

				break;
			}
		}
		flg++;
	}
	
}


void DrainageNetworkOrdering::StoreSegment(IFeatureCoverage sm, long iSegID, bool fConvert)
{
	std::vector<Coordinate> coordsPts;

	fConvert = true;

	for (long index = 0; index < m_vStreamLink.size(); ++index)
	{
		Pixel pxl = m_vStreamLink[index];

		Coordinate c;
		if (fConvert)
			c = _inRaster->georeference()->pixel2Coord(pxl);
		else
		{
			c.x = pxl.x;
			c.y = pxl.y;
			c.z = pxl.z;
		}
		coordsPts.push_back(c);
	}

	if (m_vStreamLink.size() > 0)
	{
		geos::geom::CoordinateSequence* cl1 = toCoordinateSequence(coordsPts);
		geos::geom::Geometry* geometry = sm->geomfactory()->createLineString(cl1);
		geometry->setSRID(iSegID);

		if (geometry->isValid())
		{
			sm->newFeature(geometry);
			tmpFeatureFlags.push_back(false);
		}
	
	}
}


void DrainageNetworkOrdering::CleanSegment(IFeatureCoverage smpTo, IFeatureCoverage smpFrom)
{
	long flg(0);
	for (auto feature : smpFrom)
	{
		const geos::geom::LineString* ls_geom =
			dynamic_cast<const geos::geom::LineString*>(feature->geometry().get());

		if (!ls_geom->isEmpty() && ls_geom->isValid() && !tmpFeatureFlags[flg])
		{
			geos::geom::CoordinateSequence* crdbuf1 = ls_geom->getCoordinates();

			geos::geom::CoordinateSequence* crdbufnew = crdbuf1->clone();
			
			geos::geom::Geometry* geometry = smpTo->geomfactory()->createLineString(crdbufnew);
			geometry->setSRID(ls_geom->getSRID());

			if (geometry->isValid())
			{
				smpTo->newFeature(geometry);
			}
		}
		flg++;
	}
	
}


geos::geom::CoordinateSequence*
DrainageNetworkOrdering::toCoordinateSequence(std::vector<Coordinate> cv)
{
	const geos::geom::CoordinateSequenceFactory* csf =
		_outputfeatures->geomfactory()->getCoordinateSequenceFactory();

	// Create a new Coordinate::Vect for feeding it to 
	// the CoordinateSequenceFactory
	Coordinate::Vect* vect = new Coordinate::Vect();

	size_t n = cv.size();
	vect->reserve(n); // avoid multiple reallocs

	for (size_t i = 0; i < n; ++i)
	{
		vect->push_back((cv[i])); // Coordinate copy
	}

	return csf->create(vect); // takes ownership of the vector

}


bool DrainageNetworkOrdering::IsSourceCell(Pixel pxl)
{
	//Examine the neighbor cells to locate a sourcecell,
	//return true, if no any cells flow into it.
	//Figure shows a flow path location number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	//
	int in = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 };

	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);

	if (*iterDrainage(pxl) == iDrainage && *iterFld(pxl) != 0 )
	{
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				Pixel pospxl;
				pospxl.x = pxl.x + j;
				pospxl.y = pxl.y + i;
				pospxl.z = 0;

				if (*iterFld(pospxl) == iUpFlow[in] && *iterDrainage(pospxl) > iOffDrainage)
					return false;
				in++;
			}
		}
		return true;
	}
	else
		return false;

}


void DrainageNetworkOrdering::FlagDrainageMap(long iFlag, int iLink)
{
	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);
	//Flag the drainage with the unique drainage ID
	if (iLink == iJunction)
		m_vStreamLink.resize(m_vStreamLink.size() - 1);
	for (std::vector<Pixel>::iterator pos = m_vStreamLink.begin();
		pos < m_vStreamLink.end(); ++pos)
	{
		Pixel pxl = *pos;
		pxl.z = 0;
		*iterDrainage(pxl) = iFlag;
	}
}


int DrainageNetworkOrdering::IdentifyLink(Pixel& pxl)
{
	//trace along a channel link from the given cell rc
	//the function will stop when it a junction (returns 1) or
	//an edge cell or a dead-end point is reached 

	int iLink = -2;
	Coordinate c1, c2;
	c1 = _outDrainageRaster->georeference()->pixel2Coord(pxl);

	while (iLink < 0)
	{
		m_vStreamLink.push_back(pxl);
		if (IsEdgeCell(pxl))
		{
			c2 = _inRaster->georeference()->pixel2Coord(pxl);
			m_rLength = m_rLength + rDistance(c1, c2);
			iLink = 0;
		}
		else if (IsJunction(pxl))
		{
			c2 = _inRaster->georeference()->pixel2Coord(pxl);
			m_rLength = m_rLength + rDistance(c1, c2);
			iLink = iJunction;
		}
		else
		{
			int iFlow = GetDownStreamCell(pxl);
			if (iFlow == iInValidFlow)
			{
				iLink = 0; //dead-end cell, no flow can advance at this position
			}
			else
			{
				c2 = _inRaster->georeference()->pixel2Coord(pxl);
				m_rLength = m_rLength + rDistance(c1, c2);
				c1 = c2;
			}
		}
	}
	return iLink;
}


int DrainageNetworkOrdering::GetDownStreamCell(Pixel& pxl)
{
	//Return a cell that the current given cell rc flows to,
	//otherwise, return the same cell as the given cell, This means
	//that the given cell doesn't flow to any other cell.

	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);
	pxl.z = 0;
	int iPos = *iterFld(pxl);
	switch (iPos)
	{
	case 1: 	//East
		pxl.x = pxl.x + 1;
		break;
	case 2:  //South East
		pxl.y = pxl.y + 1;
		pxl.x = pxl.x + 1;
		break;
	case 3: 	//South
		pxl.y = pxl.y + 1;
		break;
	case 4: //South West
		pxl.y = pxl.y + 1;
		pxl.x = pxl.x - 1;
		break;
	case 5:	//West
		pxl.x = pxl.x - 1;
		break;
	case 6:	//North West
		pxl.y = pxl.y - 1;
		pxl.x = pxl.x - 1;
		break;
	case 7:	//North
		pxl.y = pxl.y - 1 ;
		break;
	case 8:	//North East
		pxl.y = pxl.y - 1;
		pxl.x = pxl.x + 1;
		break;
	}

	if ( *iterDrainage(pxl) == iDrainage && *iterFld(pxl) != 0 )
		return iPos;
	else
		return 0; //invalid a flow path
}

//Get upstream index in m_vRecords
long DrainageNetworkOrdering::GetUpstream(Pixel pxl)
{
	int pos = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 };
	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);

	RecordObj ro;
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			Pixel pospxl;
			pospxl.x = pxl.x + j;
			pospxl.y = pxl.y + i;
			pospxl.z = 0;

			pos++;
			long iDrainageID = *iterDrainage(pospxl);
			if ( *iterFld(pospxl)== iUpFlow[pos - 1] )
			{
				if (iDrainageID > iDrainage) //has been identified
				{
					return FindDrainageIndex(iDrainageID);
				}
			}
		}
	}
	return 0;
}

//Find drainage index in m_vRecords
long DrainageNetworkOrdering::FindDrainageIndex(long iDrainageID)
{
	for (std::vector<RecordObj>::iterator pos = m_vRecords.begin();
		pos < m_vRecords.end(); ++pos)
	{
		RecordObj ro(*pos);
		if (ro.id == iDrainageID)
			return pos - m_vRecords.begin();
	}
	return -1;
}

bool DrainageNetworkOrdering::IsJunction(Pixel pxl)
{
	//For the specified cell at given loaction rc,
	//checks whether it is a junction,
	//Flow path number
	//	-------
	//	|6|7|8|
	//	-------
	//	|5| |1|
	//	-------
	//	|4|3|2|
	//	-------
	//
	int pos = 0;
	int iUpFlows = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 };

	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
	
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			Pixel pospxl;
			pospxl.x = pxl.x + j;
			pospxl.y = pxl.y + i;
			pospxl.z = 0;

			int iFlow = *iterFld(pospxl);
			int iDm = *iterDrainage(pospxl);

			if ( iFlow == iUpFlow[pos] && iDm > iOffDrainage )
			{
				iUpFlows++;
				if (iUpFlows > 1)
					return true;
			}
			pos++;
		}
	}
	return false;

}

bool DrainageNetworkOrdering::IsJunctionExists(Pixel node)
{
	//Find junction node in m_vJunction that matches value
	//If no such junction exists, the function returns the last
	bool IsExists = std::find(m_vJunction.begin(), m_vJunction.end(), node) != m_vJunction.end();
	if (IsExists)
		return true;
	else
		return false;
}

void DrainageNetworkOrdering::EraseJunction(Pixel pxl)
{
	std::vector<Pixel>::iterator pos = find(m_vJunction.begin(), m_vJunction.end(), pxl);
	m_vJunction.erase(pos);
}

void DrainageNetworkOrdering::StoreJunction(Pixel pxl)
{
	if (!IsJunctionExists(pxl))
		m_vJunction.push_back(pxl);
}


double DrainageNetworkOrdering::rDistance(Coordinate cd1, Coordinate cd2)
{
	double rDist;
	if (fLatLonCoords())
	{
		double rRadi = rDefaultEarthRadius;
		IConventionalCoordinateSystem projectedCsy = _inRaster->coordinateSystem().as<ConventionalCoordinateSystem>();
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


bool DrainageNetworkOrdering::fLatLonCoords()
{
	return _inRaster->coordinateSystem()->isLatLon();
}


bool DrainageNetworkOrdering::IsEdgeCell(Pixel pxl)
{
	if (pxl.y == 0 || pxl.y == _ysize - 1 ||
		pxl.x == 0 || pxl.x == _xsize - 1)
		return true;
	else
		return false;
}


void DrainageNetworkOrdering::IniParms()
{
	m_iLinkNumber = 0;
	m_iStrahler = 1;
	m_iStreve = 1;
	m_sUpstreamLink = "0";
	m_rTotalUpstreamLength = 0;
}

bool DrainageNetworkOrdering::IsAllUpstreamIdentified(Pixel node, std::vector<long>& vUpstreamPos)
{
	//* Evaluate if every upstream has been identified/traced
		//* If true, also generate upstreams IDs,
		//* calculate the total upstream length
		//* store the index of upstreams to vUpstreamPos, which indicates the location of upstreams in m_vRecords

	int pos = 0;
	int iUpFlow[9] = { 2, 3, 4, 1, 0, 5, 8, 7, 6 };
	m_rTotalUpstreamLength = 0;
	vUpstreamPos.resize(0);
	m_sUpstreamLink = "";

	PixelIterator iterFld = PixelIterator(_inFldRaster, BoundingBox(), PixelIterator::fXYZ);
	PixelIterator iterDrainage = PixelIterator(_outDrainageRaster, BoundingBox(), PixelIterator::fXYZ);

	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			pos++;
			Pixel pospxl = node;
			pospxl.x = pospxl.x + j;
			pospxl.y = pospxl.y + i;
			pospxl.z = 0;

			long iVal = *iterDrainage(pospxl);
			if (*iterFld(pospxl) == iUpFlow[pos - 1])
			{
				if (iVal == iDrainage)
				{
					return false;
				}
				else if (iVal > 0)
				{
					long index = FindDrainageIndex(iVal);
					m_rTotalUpstreamLength = m_rTotalUpstreamLength + m_vRecords[index].Length;
					QString sUpstreamLink = m_sUpstreamLink.length() != 0 ? 
						QString(",%1").arg(iVal,0,10) : QString("%1").arg(iVal,0,10);
					m_sUpstreamLink = m_sUpstreamLink + sUpstreamLink;
					vUpstreamPos.push_back(index);
				}
			}
		}
	}

	m_sUpstreamLink = CoordinateFormatString(m_sUpstreamLink);
	return true;

}


QString DrainageNetworkOrdering::CoordinateFormatString(QString crd)
{
	QString crdstring;
	crdstring = QString("{") + crd;
	crdstring = crdstring + QString("}");
	return crdstring;
}

void DrainageNetworkOrdering::AddDomainItem(long iItem)
{
	QString sUniqueID = QString("%1").arg(iItem,0,10);
	//m_dm->pdsrt()->iAdd(sUniqueID, true);
}

long DrainageNetworkOrdering::ComputeStrahlerOrder(std::vector<long> vUpstreamLink)
{
	std::vector<long> vStrahler;
	for (std::vector<long>::iterator pos = vUpstreamLink.begin();
		pos < vUpstreamLink.end(); ++pos)
	{
		long iPos(*pos);
		vStrahler.push_back(m_vRecords[iPos].Strahler);
	}

	//returns the position of the first element with max. in vValue
	std::vector<long>::iterator pos = max_element(vStrahler.begin(), vStrahler.end());
	long iMax = *pos;

	pos = min_element(vStrahler.begin(), vStrahler.end());
	long iMin = *pos;

	if (iMax > iMin)
		return iMax;
	else
		return iMax + 1;
}

long DrainageNetworkOrdering::ComputeStreveOrder(std::vector<long> vUpstreamLink)
{
	long iStreve = 0;
	for (std::vector<long>::iterator pos = vUpstreamLink.begin();
		pos < vUpstreamLink.end(); ++pos)
	{
		long iPos(*pos);
		iStreve = iStreve + m_vRecords[iPos].Streve;
	}
	return iStreve;
}

double DrainageNetworkOrdering::rComputeSlope(double rDrop, double rLength, bool fDegree)
{
	if (rLength > 0)
	{
		if (fDegree)
			return (180 / M_PI) * atanl(abs(rDrop / rLength)); //degree
		else
			return (rDrop / rLength) * 100; //percent
	}
	else
		return 0;
}


void DrainageNetworkOrdering::UpdateRecord()
{
	m_rec.id = m_iLinkNumber;
	m_rec.UpstreamLink = m_sUpstreamLink;
	m_rec.UpstreamCoord = m_vStreamLink[0];
	m_rec.DownstreamLink = 0;
	m_rec.DownstreamCoord = m_vStreamLink[m_vStreamLink.size() - 1];
	m_rec.Strahler = m_iStrahler;
	m_rec.Streve = m_iStreve;
	m_rec.Length = m_rLength;
	m_rec.TotalUpstreamLength = m_rTotalUpstreamLength;
	m_rec.TostreamCoord = m_vStreamLink[m_vStreamLink.size() - 1];
}


void DrainageNetworkOrdering::CreateTable(long maxStrahler)
{
	// prepare table
	
	IFlatTable newTable;
	newTable.prepare();

	//newTable->addColumn("StreamID", _orderDomain);
	newTable->addColumn("UpstreamLinkID", IlwisObject::create<IDomain>("text"),true);
	//newTable->addColumn("UpstreamCoord", IlwisObject::create<IDomain>("text"));
	newTable->addColumn("UpstreamCoord", IlwisObject::create<IDomain>("coordinate"));
	newTable->addColumn("UpstreamElevation", IlwisObject::create<IDomain>("value"));
	newTable->addColumn("DownstreamLinkID", IlwisObject::create<IDomain>("value"));

	ColumnDefinition& coldef0 = newTable->columndefinitionRef("DownstreamLinkID");
	coldef0.datadef().range(new NumericRange(1, 32767, 1));

	//newTable->addColumn("DownstreamCoord", IlwisObject::create<IDomain>("text"), true);
	newTable->addColumn("DownstreamCoord", IlwisObject::create<IDomain>("coordinate"));
	newTable->addColumn("DownstreamElevation", IlwisObject::create<IDomain>("value"));
	newTable->addColumn("ElevationDifference", IlwisObject::create<IDomain>("value"));
	newTable->addColumn("Strahler", IlwisObject::create<IDomain>("value"), true);

	ColumnDefinition& coldef1 = newTable->columndefinitionRef("Strahler");
	coldef1.datadef().range(new NumericRange(1, 32767, 1));

	newTable->addColumn("Shreve", IlwisObject::create<IDomain>("value"), true);
	ColumnDefinition& coldef2 = newTable->columndefinitionRef("Shreve");
	coldef2.datadef().range(new NumericRange(1, 32767, 1));

	newTable->addColumn("Length", IlwisObject::create<IDomain>("value"));
	newTable->addColumn("StraightLength", IlwisObject::create<IDomain>("value"));

	newTable->addColumn("SlopeAlongDrainagePerc", IlwisObject::create<IDomain>("value"));
	ColumnDefinition& coldef3 = newTable->columndefinitionRef("SlopeAlongDrainagePerc");
	coldef3.datadef().range(new NumericRange(0, 1e10, 0.01));

	newTable->addColumn("SlopeAlongDrainageDegree", IlwisObject::create<IDomain>("value"));
	ColumnDefinition& coldef4 = newTable->columndefinitionRef("SlopeAlongDrainageDegree");
	coldef4.datadef().range(new NumericRange(0, 1e10, 0.001));

	newTable->addColumn("SlopeDrainageStraightPerc", IlwisObject::create<IDomain>("value"));
	ColumnDefinition& coldef5 = newTable->columndefinitionRef("SlopeDrainageStraightPerc");
	coldef5.datadef().range(new NumericRange(0, 1e10, 0.01));

	newTable->addColumn("SlopeDrainageStraightDegree", IlwisObject::create<IDomain>("value"));
	ColumnDefinition& coldef6 = newTable->columndefinitionRef("SlopeDrainageStraightDegree");
	coldef6.datadef().range(new NumericRange(0, 1e10, 0.001));

	newTable->addColumn("Sinuosity", IlwisObject::create<IDomain>("value"));
	ColumnDefinition& coldef7 = newTable->columndefinitionRef("Sinuosity");
	coldef7.datadef().range(new NumericRange(0, 1e10, 0.001));

	newTable->addColumn("TotalUpstreamAlongDrainageLength", IlwisObject::create<IDomain>("value"));

	//newTable->addColumn("TostreamCoord", IlwisObject::create<IDomain>("text"), true);
	newTable->addColumn("TostreamCoord", IlwisObject::create<IDomain>("coordinate"));

	if (maxStrahler > 0) {
		IThematicDomain strahlerDomain;
		strahlerDomain.prepare();
		for (long i = 1; i <= maxStrahler; ++i) {
			strahlerDomain->addItem(new ThematicItem(QString("Strahler %1").arg(i), "", ""));
		}
		newTable->addColumn("StrahlerClass", strahlerDomain, true);
	}
	_outputTable = newTable;
}


void DrainageNetworkOrdering::FillTableRecords()
{
	long iMaxOrder = 0;
	for (std::vector<RecordObj>::iterator pos = m_vRecords.begin(); pos < m_vRecords.end(); ++pos)
	{
		RecordObj rec(*pos);
		if (rec.Strahler > iMaxOrder)
			iMaxOrder = rec.Strahler;
	}

	CreateTable(iMaxOrder);
	_outputTable->addColumn(_outDrainageRaster->primaryKey(), _orderDomain);
	//_outDrainageRaster->setAttributes(_outputTable);

	Coordinate c1, c2;
	long index = 0;
	double rUpstreamHeight, rDownstreamHeight;

	quint32 record = 0;
	quint32 totalcount = m_vRecords.size();

	PixelIterator iterDEM = PixelIterator(_inRaster, BoundingBox(), PixelIterator::fXYZ);

	for (std::vector<RecordObj>::iterator pos = m_vRecords.begin(); pos < m_vRecords.end(); ++pos)
	{
		RecordObj rec(*pos);
		record = rec.id;

		QString id = QString::number(record);

		if (id != "")
		{
			if (_idrange->contains(id))
			{
				++totalcount;
				id = QString::number(totalcount);
			}
			*_idrange << id;
		}

		record = record - 1;

		//_outputTable->setCell("StreamID", record, QVariant(record));

		_outputTable->setCell("UpstreamLinkID", record, QVariant(rec.UpstreamLink));

		Pixel pxl = rec.UpstreamCoord;
		Coordinate c1 = _inRaster->georeference()->pixel2Coord(pxl);
		QString crd = c1.toString();
		_outputTable->setCell("UpstreamCoord", record, QVariant(CoordinateFormatString(crd)));

		rUpstreamHeight = *iterDEM(pxl);
		_outputTable->setCell("UpstreamElevation", record, QVariant(rUpstreamHeight));
		_outputTable->setCell("DownstreamLinkID", record, QVariant(rec.DownstreamLink));

		pxl = rec.DownstreamCoord;
		Coordinate c2 = _inRaster->georeference()->pixel2Coord(pxl);
		crd = c2.toString();
		_outputTable->setCell("DownstreamCoord", record, QVariant(CoordinateFormatString(crd)));

		rDownstreamHeight = *iterDEM(pxl);
		_outputTable->setCell("DownstreamElevation", record, QVariant(rDownstreamHeight));

		double rDrop = rUpstreamHeight - rDownstreamHeight;
		_outputTable->setCell("ElevationDifference", record, QVariant(rDrop));

		_outputTable->setCell("Strahler", record, QVariant(rec.Strahler));

		_outputTable->setCell("Shreve", record, QVariant(rec.Streve));
		_outputTable->setCell("Length", record, QVariant(rec.Length));
		double rStraightLength = rDistance(c1, c2);
		_outputTable->setCell("StraightLength", record, QVariant(rStraightLength));

		double rSlop = rComputeSlope(rDrop, rec.Length, false);
		_outputTable->setCell("SlopeAlongDrainagePerc", record, QVariant(rSlop));

		rSlop = rComputeSlope(rDrop, rec.Length, true);
		_outputTable->setCell("SlopeAlongDrainageDegree", record, QVariant(rSlop));

		rSlop = rComputeSlope(rDrop, rStraightLength, false);
		_outputTable->setCell("SlopeDrainageStraightPerc", record, QVariant(rSlop));

		rSlop = rComputeSlope(rDrop, rStraightLength, true);
		_outputTable->setCell("SlopeDrainageStraightDegree", record, QVariant(rSlop));

		double rSinuosity = rComputeSinuosity(rec.Length, rStraightLength);
		_outputTable->setCell("Sinuosity", record, QVariant(rSinuosity));

		_outputTable->setCell("TotalUpstreamAlongDrainageLength", record, QVariant(rec.TotalUpstreamLength));

		Pixel pxl1 = rec.TostreamCoord;
		Coordinate c = _inRaster->georeference()->pixel2Coord(pxl1);
		QString crdc = c.toString();

		_outputTable->setCell("TostreamCoord", record, QVariant(CoordinateFormatString(crdc)));

		//_outputTable->setCell("StrahlerClass", record, QVariant(rec.Strahler));
		_outputTable->setCell("StrahlerClass", record, QVariant(QString("Strahler %1").arg(rec.Strahler)));

		_outputTable->setCell(_outDrainageRaster->primaryKey(), record, QVariant(record));
		index++;
	}
	m_vRecords.resize(0);
}


void DrainageNetworkOrdering::DownstreamLink(std::vector<long> vPos)
{
	for (std::vector<long>::iterator pos = vPos.begin();
		pos < vPos.end(); ++pos)
	{
		long iPos(*pos);
		m_vRecords[iPos].DownstreamLink = m_iLinkNumber;
	}
}

double DrainageNetworkOrdering::rComputeSinuosity(double rLength, double rStraightLenght)
{
	if (rStraightLenght > 0)
		return rLength / rStraightLenght;
	else
		return 0;
}