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

#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "raster.h"
#include "representation.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "drawerattributesetter.h"


using namespace Ilwis;
using namespace Ui;

DrawerAttributeSetter::DrawerAttributeSetter(const IOOptions &options)
{

}

DrawerAttributeSetter::~DrawerAttributeSetter()
{

}

//FeatureDrawing DrawerAttributeSetter::setSpatialAttributes(const SPFeatureI &, QVector<QVector3D> &, QVector<QVector3D> &) const
//{
//    return  FeatureDrawing(itUNKNOWN);
//}

//void DrawerAttributeSetter::setColorAttributes(const VisualAttribute &,
//                                               const QVariant &, const QColor& ,
//                                               const FeatureDrawing &,
//                                               std::vector<VertexColor> &) const
//{

//}


