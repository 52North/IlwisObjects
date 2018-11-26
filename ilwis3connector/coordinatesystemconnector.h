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

#ifndef COORDINATESYSTEMCONNECTOR_H
#define COORDINATESYSTEMCONNECTOR_H

namespace Ilwis {
namespace Ilwis3{
class CoordinateSystemConnector : public Ilwis3Connector
{
public:
    CoordinateSystemConnector(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *data, const IOOptions &options=IOOptions());
    bool storeBinaryData(IlwisObject* );

    static bool canUse(const Ilwis::Resource &resource, const UPCatalogConnector &container);

    IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true, const Ilwis::IOOptions &options=IOOptions());

    QString format() const;
private:
    GeodeticDatum *getDatum(Ilwis::IEllipsoid &ellipsoid);
    IProjection getProjection(Ilwis::ConventionalCoordinateSystem *csycc);
    IEllipsoid getEllipsoid();

    static QString prjParam2IlwisName(Projection::ProjectionParamValue parm);
};
}
}

#endif // COORDINATESYSTEMCONNECTOR_H
