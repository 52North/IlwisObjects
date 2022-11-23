#ifndef NETCDFRASTERCONNECTOR_H
#define NETCDFRASTERCONNECTOR_H
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

#include "ncFile.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;

    namespace NetCdf {

        class NetCdfRasterConnector : public IlwisObjectConnector
        {
        public:
            NetCdfRasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions& options = IOOptions());
            IlwisObject *create() const;

            bool loadMetaData(IlwisObject*obj, const IOOptions & options);
            static ConnectorInterface *create(const Ilwis::Resource &resource, bool load, const IOOptions& options = IOOptions());
            bool loadData(IlwisObject *, const IOOptions &options = IOOptions());
            QString provider() const;

        private:
            quint32 _version;
            IRasterCoverage _dataRaster;

            void getValue(const std::vector<size_t> &index, const std::vector<size_t> &count, int maxX, int maxY, bool xy, netCDF::NcVar &var, std::vector<double> &values);
            template<class T>
            void getValues(const std::vector<size_t> &index, const std::vector<size_t> &count, int maxX, int maxY, bool xy, netCDF::NcVar &var, std::vector<T> &data, std::vector<double> &values);
        };
    }
}
#endif // NETCDFRASTERCONNECTOR_H
