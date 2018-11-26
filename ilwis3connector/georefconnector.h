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

#ifndef GEOREFCONNECTOR_H
#define GEOREFCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class GeorefConnector : public Ilwis3Connector
{
public:
    GeorefConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *obj, const IOOptions &options);
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    QString format() const;
private:
    bool loadGeoref(const IniFile &odf, IlwisObject *data);
    void createGeoreference(const IniFile &odf, GeoReference *grf) const;
    bool loadGeorefCorners(const IniFile &odf, Ilwis::IlwisObject *data);
    bool loadGeorefTiepoints(const IniFile &odf, GeoReference *grf);
};
}
}

#endif // GEOREFCONNECTOR_H
