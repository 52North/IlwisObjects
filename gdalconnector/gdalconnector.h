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

#ifndef GDALCONNECTOR_H
#define GDALCONNECTOR_H

#include "gdalconnector_global.h"

namespace Ilwis{
namespace Gdal {

class GDALCONNECTORSHARED_EXPORT GdalConnector : public IlwisObjectConnector
{
public:
    GdalConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    virtual ~GdalConnector();
    static IlwisTypes ilwisType(const QString &name);

    bool loadMetaData(IlwisObject* data,const IOOptions&);

    QString provider() const;
    void format(const QString&);
    QString format() const;
    OGRLayerH getLayerHandle() const;
    bool isReadOnly() const;
protected:
    static GDALDataType ilwisType2GdalType(IlwisTypes tp) ;
    static OGRFieldType ilwisType2GdalFieldType(IlwisTypes tp);
    QString constructOutputName(GDALDriverH hdriver) const;
    void getTypes(const std::multimap<QString, DataFormat>& formats, IlwisTypes & tp, IlwisTypes & extendedType) const;
    QUrl _fileUrl;
    QString _internalPath;
    QString _gdalShortName;
    bool _readOnly = true;
    GdalHandle* _handle;
};
}
}

#endif // GDALCONNECTOR_H
