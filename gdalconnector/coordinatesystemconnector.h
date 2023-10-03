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

namespace Ilwis{
namespace Gdal{

class CoordinateSystemConnector : public GdalConnector
{
public:
    CoordinateSystemConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);

   // static bool canUse(const Ilwis::Resource &resource);

    IlwisObject *create() const;
    static ConnectorInterface *create(const Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    void extractUserDefinedEllipsoid(ConventionalCoordinateSystem *csyp, OGRSpatialReferenceH srshandle);
    
private:
    void setProjectionParameter(OGRSpatialReferenceH handle, const char *wkt, Projection::ProjectionParamValue parmType, IProjection &projection);
    std::vector<double> getShifts(QString & fileName, QString & datumName) const;
    void setDatum(ConventionalCoordinateSystem *csyp, OGRSpatialReferenceH srshandle);
};
}
}

#endif // COORDINATESYSTEMCONNECTOR_H
