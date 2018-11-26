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
#include "numericrange.h"
#include "numericdomain.h"
#include "table.h"
#include "factory.h"
#include "abstractfactory.h"
#include "featurefactory.h"
#include "featurecoverage.h"
#include "feature.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "internalfeaturecoverageconnector.h"

using namespace Ilwis;
using namespace Internal;

InternalFeatureCoverageConnector::InternalFeatureCoverageConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) : IlwisObjectConnector(resource, load, options)
{

}

bool InternalFeatureCoverageConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    return true;
}

bool InternalFeatureCoverageConnector::loadData(Ilwis::IlwisObject *, const IOOptions & )
{
    _binaryIsLoaded = true;
    return true;
}

QString InternalFeatureCoverageConnector::provider() const
{
    return "internal";
}

ConnectorInterface *InternalFeatureCoverageConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new  InternalFeatureCoverageConnector(resource, load, options);
}

IlwisObject *InternalFeatureCoverageConnector::create() const
{
    return new FeatureCoverage();
}
