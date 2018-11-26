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
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "domainitem.h"
#include "ilwisdata.h"
#include "domain.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericrange.h"
#include "range.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "gdalproxy.h"
#include "dataformat.h"
#include "gdalconnector.h"
#include "numericdomain.h"
#include "domainconnector.h"


using namespace Ilwis;
using namespace Gdal;

ConnectorInterface *DomainConnector::create(const Resource& resource, bool load, const IOOptions &options) {
    return new DomainConnector(resource, load, options);

}

DomainConnector::DomainConnector(const Resource& resource, bool load, const IOOptions &options) : GdalConnector(resource,load, options)
{
}

bool DomainConnector::loadMetaData(IlwisObject *data, const IOOptions &options){
    bool ret = true;
    if (!GdalConnector::loadMetaData(data, options))
        ret = false;

    if (type() == itUNKNOWN) {
        kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_FOR_2).arg("Domain type",data->name()));
        ret = false;
    }

    if (ret == true){
        if ( type() == itNUMERICDOMAIN) {
            ret = handleValueDomains(data);
        } else if ( type() == itITEMDOMAIN) {
            ret = handleThematicDomains(data);
        }
    }
    QFileInfo fileinf (sourceRef().toLocalFile());
    gdal()->closeFile(fileinf.absoluteFilePath(), data->id());
    return ret;

}

bool DomainConnector::handleThematicDomains(IlwisObject* ) {
    //TODO: handle thematic raster domains
    return false;
}

bool DomainConnector::handleValueDomains(IlwisObject* data) {
    //NumericDomain *vdata = static_cast<NumericDomain*>(data);
    //TODO: adjust the range
    return true;
}

IlwisObject *DomainConnector::create() const
{
    //TODO: other domain types time, identifier
    //TODO: what about other system domain types that are value derived.
    if ( type() == itNUMERICDOMAIN)
        return new NumericDomain(_resource);
    //else if ( type() == itTHEMATICDOMAIN) {
    //    return new ItemDomain<ThematicItem>(_resource);
    //}
    return 0;
}
