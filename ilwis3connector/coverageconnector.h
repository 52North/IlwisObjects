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

#ifndef COVERAGECONNECTOR_H
#define COVERAGECONNECTOR_H

namespace Ilwis {
namespace Ilwis3{

class TableConnector;

class CoverageConnector : public Ilwis3Connector
{
public:
    CoverageConnector(const Resource& resource, bool load=true,const IOOptions& options=IOOptions());
protected:
    bool getRawInfo(const QString &range, double &vmin, double &vmax, double &scale, double &offset) const;
    virtual void calcStatistics(const IlwisObject *obj,NumericStatistics::PropertySets set) const = 0;
    bool loadMetaData(Ilwis::IlwisObject *data, const IOOptions &options);
    bool storeMetaData(IlwisObject *obj, IlwisTypes type, const Ilwis::DataDefinition &datadef, const QString &baseName=sUNDEF) ;
   // bool storeBinaryData(IlwisObject *obj, IlwisTypes tp, const QString &baseName);
    TableConnector *createTableStoreConnector(ITable &attTable, Coverage *coverage, IlwisTypes tp, const QString &baseName);

    RawConverter _converter;
    DataDefinition determineDataDefinition(const Ilwis::ODF &odf, const IOOptions &) const;
protected:
    QString _domainName;
    QString _domainInfo;
    QString _csyName;
    int _itemCount = 0;
private:
    ITable prepareAttributeTable(const QString &file, const QString& basemaptype, const IOOptions &) const;

    /**
     * @brief addToMasterCatalog will check if the mastercatalog has a resource under this name, and add it if it doesn't;
     * This is a helper function, its purpose is to workaround there where the ilwisobjects core code isn't properly opening files that aren't registered in the mastercatalog
     * @param filename the file to add as an ODFItem-resource
     * @param it the type under which the item is expected to be found
     */
    void addToMasterCatalog(QString filename, IlwisTypes it) const;

};
}
}

#endif // COVERAGECONNECTOR_H
