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
#include "ilwis.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "georefimplementation.h"
#include "factory.h"
#include "abstractfactory.h"
#include "coordinatesystem.h"
#include "georeference.h"
#include "simpelgeoreference.h"
#include "cornersgeoreference.h"
#include "controlpoint.h"
#include "ctpgeoreference.h"
#include "eigen3/Eigen/LU"
#include "mathhelper.h"
#include "planarctpgeoreference.h"
#include "undeterminedgeoreference.h"
#include "georefimplementationfactory.h"

using namespace Ilwis;

GeoRefImplementationFactory::GeoRefImplementationFactory() : AbstractFactory("georefimplementationfactory","ilwis","")
{
}

void GeoRefImplementationFactory::addCreator(const QString &name, CreateGeoRefImplementation func)
{
    auto iter = _implCreators.find(name);
    if ( iter == _implCreators.end()) {
        _implCreators[name] = func;
    }
}

GeoRefImplementation *GeoRefImplementationFactory::create(const QString& name)  {
    auto iter = _implCreators.find(name);
    if ( iter != _implCreators.end()) {
        return _implCreators[name]();
    }
    return 0;
}

bool GeoRefImplementationFactory::prepare()
{
    AbstractFactory::prepare();

    addCreator("simpel",SimpelGeoReference::create);
    addCreator("corners",CornersGeoReference::create);
    addCreator("undetermined",UndeterminedGeoReference::create);
    addCreator("tiepoints",PlanarCTPGeoReference::create);

    return true;
}


