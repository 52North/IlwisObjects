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

#ifndef POSTGRESQLRASTERCONNECTOR_H
#define POSTGRESQLRASTERCONNECTOR_H

namespace Ilwis {

class RasterCoverage;

namespace Postgresql {

class PostgresqlRasterConnector : public PostgresqlConnector
{
public:
    PostgresqlRasterConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    ~PostgresqlRasterConnector();

    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *data, const IOOptions &options=IOOptions());
    bool store(IlwisObject* data, const IOOptions &options);

private:
    bool moveIndexes(quint32 &linesPerBlock, quint32 &linesLeft, int &gdalindex);
    IlwisTypes getStoreType(QString pixel_type, IDomain & dom) const;
    static char hex2dec(const char * str);
    static double getNodataVal(QString val, QString & pixel_type);
    std::vector<IlwisTypes> _pixelTypes;
    std::vector<double> _noDataValues;
    std::vector<bool> _hasNodataValue;
    quint32 _nrBands;
    bool _outDb;
    bool _newStyleST_AsBinary;
    quint32 _x_pixels_tile;
    quint32 _y_pixels_tile;
    const int _headerSize;
    const int _bandHeaderSize;
    QString _srid;
};
}
}
#endif // POSTGRESQLRASTERCONNECTOR_H
