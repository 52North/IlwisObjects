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

#ifndef WFSPARSINGCONTEXT_H
#define WFSPARSINGCONTEXT_H

#include "wfsconnector_global.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsParsingContext
{
public:
    WfsParsingContext();

    QString srsName() const;
    int srsDimension() const;
    QString geometryAtttributeName() const;
    QMap<QString,QString> namespaceMappings() const;
    QString featureType()const;
    Resource resource() const;
    QString currentItem() const;

    void setSrsName(QString srsName);
    void setSrsDimension(int srsDimension);
    void setGeometryAtttributeName(QString geometryAtttributeName);
    void addNamespaceMapping(QString prefix, QString uriNamespace);
    void setFeatureType(QString featureType);
    void setResource(Resource resource);
    void setCurrentItem(QString item);

private:
    QString _srsName;
    int _srsDimension;
    QString _currentItem;
    QString _featureType;
    QString _geometryAttributeName;
    QMap<QString,QString> _namespaceMappings;
    Resource _resource;
};
}
}

#endif // WFSPARSINGCONTEXT_H
