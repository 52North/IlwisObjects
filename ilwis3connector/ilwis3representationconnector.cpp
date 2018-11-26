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
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "inifile.h"
#include "ilwis3connector.h"
#include "representation.h"
#include "ilwis3representationconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

Ilwis3RepresentationConnector::~Ilwis3RepresentationConnector()
{

}

Ilwis3RepresentationConnector::Ilwis3RepresentationConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options) :
    Ilwis3Connector(resource, load, options)
{

}

bool Ilwis3RepresentationConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    Ilwis3Connector::loadMetaData(data, options);

    QString type = _odf->value("Representation","Type");
    Representation *rpr = static_cast<Representation *>(data);
    if ( type == "RepresentationGradual"){
        return parseRprGradual(rpr);
    }else if ( type == "RepresentationClass") {
        return parseRprClass();
    }
    return false;
}

bool Ilwis3RepresentationConnector::storeMetaData(IlwisObject *obj, const IOOptions &options)
{
    return false;
}

IlwisObject *Ilwis3RepresentationConnector::create() const
{
    return new Representation();
}

ConnectorInterface *Ilwis3RepresentationConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new Ilwis3RepresentationConnector(resource, load, options);
}

QString Ilwis3RepresentationConnector::format() const
{
    return "representation";
}

bool Ilwis3RepresentationConnector::parseRprClass()
{
    return false;
}

bool Ilwis3RepresentationConnector::parseRprGradual(Representation* rpr)
{
    bool ok;
    int boundaryWidth = _odf->value("Representation","BoundaryWidth").toInt(&ok);
    if ( ok){

    }
    return false;
}

