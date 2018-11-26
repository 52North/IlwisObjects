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

#ifndef DOMAINVALUECONNECTOR_H
#define DOMAINVALUECONNECTOR_H

#include <QSettings>
#include <QUrl>
//#include "ilwis3connector.h"

namespace Ilwis {
namespace Ilwis3{
class DomainConnector : public Ilwis3Connector
{
public:
    DomainConnector(const Resource& resource, bool load=true,const IOOptions& options=IOOptions());
    bool loadMetaData(Ilwis::IlwisObject *data, const IOOptions&);
    bool storeMetaData(Ilwis::IlwisObject *data, const IOOptions &options=IOOptions());
    IlwisObject *create() const;

    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    QString format() const;
private:
    bool handleValueDomains(IlwisObject *data);
    /*!
     \brief transfers the raw, codes (if present) and description (if present) to the internal storage of the thematic domain.

     The raw values become the id's of the thematic item. Each raw value is offset with a multiplier to ensure that all different thematic domains have thematic items with different values.
     Thematic domains are fully loaded at creation time because of their limited size(relatively).

     \param data the ilwis object to be initialized. It must be created as a thematicdomain
     \return bool any error when creating the domain will return a false. Relevant errors can be found in the issue stack. The method returns true when the object is properly initialized
    */
    bool handleItemDomains(IlwisObject *data);
    bool handleIdDomain(IlwisObject *);
    bool handleValueObjects(const QString& section,double &vmin, double &vmax, double &resolution, QString &parentDomain) const;
    Ilwis::Range *handleValueDomainObjects() const;
    bool range2MinMax(const QString &range, double &vmin, double &vmax) const;
    bool minMax2minMax(const QString &minmax, double &vmin, double &vmax) const;
    QString parseDomainInfo(const QString &inf) const;
    bool storeMetaDataSortDomain(Ilwis::Domain *dom, IlwisTypes tp) ;
    IlwisObject *fromValueRange() const;
};
}

}
#endif // DOMAINVALUECONNECTOR_H
