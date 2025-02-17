#ifndef NETCDFCATALOGCONNECTOR_H
#define NETCDFCATALOGCONNECTOR_H
#pragma once

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

#include <map>
#include <netcdf>
#include "size.h"

namespace Ilwis {
    class CoordinateSystem;
    typedef IlwisData<CoordinateSystem> ICoordinateSystem;
    class GeoReference;

    typedef IlwisData<GeoReference> IGeoReference;

    namespace NetCdf {

        class NetCdfCatalogExplorer : public CatalogExplorer

        {
        friend class NetCdfRasterConnector;

        public:
            NetCdfCatalogExplorer(const Ilwis::Resource &resource, const IOOptions& options = IOOptions());

            std::vector<Resource> loadItems(const Ilwis::IOOptions &options = IOOptions());
            bool canUse(const Resource& resource) const;
            QString provider() const;

            static Ilwis::CatalogExplorer *create(const Ilwis::Resource &resource, const Ilwis::IOOptions &options);


            
        private:
            struct BandData {
                            QString _file;
                            QString _bandPath;
                            QString _bandName;
                            quint32 _nodata;
                            std::map<QString, QString> _metadata;
                            QString _dataUrl;
                            int _resolution;
                            Time _time;
                        };
            QFileInfo toLocalFile(const QUrl &datasource) const { return QFileInfo(); }

            NEW_CATALOGEXPLORER(NetCdfCatalogExplorer);
            IlwisObject *createType(IlwisTypes tp);
            static std::vector<Resource> createResources(const QUrl &url) ;
            static std::map<QString, QVariant> getGlobals(const std::multimap<std::string, netCDF::NcGroupAtt> &attributes) ;
            static void setMetaData(const std::map<QString, QVariant> &globals, Resource &res);
            static std::map<QString, std::pair<unsigned int, QString> > getDimensions(const std::multimap<std::string, netCDF::NcVar> &vars, const netCDF::NcVar &var) ;
            static ICoordinateSystem getCoordSystem(const std::multimap<std::string, netCDF::NcVar> &vars, const std::pair<std::string, netCDF::NcVar> &var) ;
            static Ilwis::IGeoReference getGrfs(const QString &path, const Size<> sz, const ICoordinateSystem &csy, std::vector<Ilwis::IGeoReference> &grfs) ;
            static void setRasterGeometryinResource(const Ilwis::ICoordinateSystem &csy, const Ilwis::IGeoReference &grf, const Size<> &sz, Ilwis::Resource &res) ;
            static QString getProjection(const std::string &gridMapName) ;
        };
    }
}
#endif // NETCDFCATALOGCONNECTOR_H
