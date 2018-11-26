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

#ifndef WFSFEATURECONNECTOR_H
#define WFSFEATURECONNECTOR_H

#include "wfsconnector_global.h"

#include "kernel.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"

#include "wfsparsingcontext.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeatureConnector : public IlwisObjectConnector
{
public:
    static ConnectorInterface *create(const Resource &resource, bool load=true, const IOOptions &options=IOOptions());
    WfsFeatureConnector(const Resource &resource, bool load=true, const IOOptions &options=IOOptions());

    QString provider() const;
    IlwisObject *create() const;
    static IlwisTypes ilwisType(const QString &name);

protected:
    bool loadMetaData(IlwisObject* data, const IOOptions&options);
    bool loadData(IlwisObject* data,const IOOptions& options);

private:
    WfsParsingContext _context;
};
}
}
#endif // WFSFEATURECONNECTOR_H


