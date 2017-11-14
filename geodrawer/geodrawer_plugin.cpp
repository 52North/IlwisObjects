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


