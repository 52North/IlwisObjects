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

#include <QString>

#include "kernel.h"
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "controlpoint.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "ctpgeoreference.h"

using namespace Ilwis;


//-------------------------------------------------------
CTPGeoReference::CTPGeoReference(const QString& type) : GeoRefImplementation(type)
{
}

bool CTPGeoReference::isValid() const
{
    return _controlPoints.size() >= 2;
}

ControlPoint &CTPGeoReference::controlPoint(quint32 index)
{
    _invalidCP = ControlPoint(true);
    if ( index < _controlPoints.size())
        return _controlPoints[index];
    return _invalidCP;

}

quint32 CTPGeoReference::nrControlPoints() const
{
    return (quint32)_controlPoints.size();
}

qint32 CTPGeoReference::index(const Pixel &pix) const
{
    for(qint32 i=0; i < (qint32)_controlPoints.size(); ++i) {
        Pixel cp( _controlPoints[i].gridLocation());
        if ( cp == pix)
            return i;
    }
    return iUNDEF;
}

qint32 CTPGeoReference::index(const Coordinate &crd) const
{
    for(qint32 i=0; i < (qint32)_controlPoints.size(); ++i) {
        Coordinate c( _controlPoints[i]);
        if ( c == crd)
            return i;
    }
    return iUNDEF;
}

qint32 CTPGeoReference::setControlPoint(const ControlPoint &pnt)
{
    qint32 rec = index(Pixel(pnt.gridLocation()));
    if ( rec == iUNDEF) {
        _controlPoints.push_back(pnt);
        return (qint32)_controlPoints.size() - 1;
    }
    controlPoint(rec) = pnt;
    return rec;
}

void Ilwis::CTPGeoReference::removeControlPoint(int index)
{
    if ( index >= 0 && index < _controlPoints.size())
        _controlPoints.erase(_controlPoints.begin() + index);
}

int CTPGeoReference::compute()
{
    return 1;
}

QString CTPGeoReference::grfType() const {
	return CTPGeoReference::typeName();
}

QString CTPGeoReference::typeName()
{
    return "CTPGeoReference";
}

void CTPGeoReference::copyTo(GeoRefImplementation *impl){
    GeoRefImplementation::copyTo(impl);

    CTPGeoReference *ctpgrf = static_cast<CTPGeoReference *>(impl);

    ctpgrf->_controlPoints = _controlPoints;
    ctpgrf->_invalidCP = _invalidCP;
}

void CTPGeoReference::subPixelPrecision(bool yesno) {
    _subPixelPrecision = yesno;
}
bool CTPGeoReference::subPixelPrecision() const {
    return _subPixelPrecision;
}


