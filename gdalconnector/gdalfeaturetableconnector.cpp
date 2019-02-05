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
#include "ilwisdata.h"
#include "gdalproxy.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "gdalfeaturetableconnector.h"
#include "domain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "coverage.h"
#include "attributedefinition.h"
#include "featurecoverage.h"
#include "featureiterator.h"
#include "feature.h"
#include "basetable.h"
#include "flattable.h"
#include "attributetable.h"
#include "gdaltableloader.h"

using namespace Ilwis;
using namespace Gdal;

GdalFeatureTableConnector::GdalFeatureTableConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : GdalConnector(resource, load, options){}

ConnectorInterface *GdalFeatureTableConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new GdalFeatureTableConnector(resource, load, options);

}

IlwisObject* GdalFeatureTableConnector::create() const{
    bool astable = ioOptions().contains("asflattable");
    if ( astable == false && hasType(_resource.extendedType(), itFEATURE))
        return new AttributeTable(_resource);
    return new FlatTable(_resource);
}


bool GdalFeatureTableConnector::loadMetaData(IlwisObject* data, const IOOptions& options){
    if(!GdalConnector::loadMetaData(data, options))
        return false;

    OGRLayerH hLayer = getLayerHandle();
    if ( hLayer){
        GdalTableLoader loader;
        loader.loadMetaData((Table *)data, hLayer);
    }
    return true;
}

bool GdalFeatureTableConnector::storeMetaData(Ilwis::IlwisObject *obj){
    return true;
}

bool GdalFeatureTableConnector::loadData(IlwisObject * data, const IOOptions &){
    if(!GdalConnector::loadMetaData(data, IOOptions())) // ??
        return false;

    Table *attTable = static_cast<Table *>(data);

    OGRLayerH hLayer = getLayerHandle();
    bool ok = true;
    if ( hLayer) {
        std::vector<QVariant> record(attTable->columnCount());

        GdalTableLoader loader;
        loader.setColumnCallbacks(attTable, hLayer);
        OGRFeatureH hFeature = 0;
        gdal()->resetReading(hLayer);
        //each FEATURE
        try {
            quint32 index = 0;
            while( (hFeature = gdal()->getNextFeature(hLayer)) != NULL){
                loader.loadRecord(attTable, hFeature, record);
                attTable->record(index++, record);
                gdal()->destroyFeature( hFeature );
            }
            _binaryIsLoaded = true;
        } catch (FeatureCreationError& ) {
            gdal()->destroyFeature( hFeature );
            ok = false;
        }
    }
    gdal()->closeFile(_fileUrl.toLocalFile(), data->id());
    return ok;
}



bool GdalFeatureTableConnector::storeBinaryData(IlwisObject* obj){
    return true;
}
