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

//#include "geodrawer.h"

#include <qqml.h>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "georeference.h"
#include "iooptions.h"
#include "coveragedisplay/layermanager.h"
#include "coveragedisplay/coveragelayermodel.h"
#include "featurelayermodel.h"
#include "rasterlayermodel.h"
//#include "layersview.h"
#include "geodrawer_plugin.h"

void GeodrawerPlugin::registerTypes(const char *uri) 
{
    // @uri n52.org.ilwisobjects
   // qmlRegisterType<LayersView>(uri, 1, 0, "LayersView");

   // Ilwis::Geodrawer::DrawerFactory::registerDrawer("SelectionDrawer", Ilwis::Geodrawer::SelectionDrawer::create);

    Ilwis::kernel()->issues()->log("Loaded module GeoDrawer",Ilwis::IssueObject::itMessage);
	qmlRegisterType<Ilwis::Ui::FeatureLayerModel>("FeatureLayerModel", 1, 0, "FeatureLayerModel");
	qmlRegisterType<Ilwis::Ui::RasterLayerModel>("RasterLayerModel", 1, 0, "RasterLayerModel");
}


