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

#ifndef ILWIS3REPRESENTATIONCONNECTOR_H
#define ILWIS3REPRESENTATIONCONNECTOR_H

namespace Ilwis {
namespace Ilwis3 {
class Ilwis3RepresentationConnector : public Ilwis3Connector
{
public:
     ~Ilwis3RepresentationConnector();

    Ilwis3RepresentationConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *obj, const Ilwis::IOOptions &options);
    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    QString format() const;

private:
    bool parseRprClass();
    bool parseRprGradual(Ilwis::Representation *rpr);
};
}
}

#endif // ILWIS3REPRESENTATIONCONNECTOR_H
