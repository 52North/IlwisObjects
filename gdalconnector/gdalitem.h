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

#ifndef GDALITEM_H
#define GDALITEM_H

namespace Ilwis {
namespace Gdal {
class GDALItems : public std::set<Resource>
{
public:
    GDALItems(const QUrl& path, const QFileInfo &localFile, IlwisTypes tp, IlwisTypes extTypes);
    GDALItems(const QFileInfo &localContainerFile, IlwisTypes tp, IlwisTypes extTypes);

    quint64 extractNameAndDomain(const QString &parts, QString &shortname);

private:
	quint64 addItem(GdalHandle* handle, const QUrl &urll, const QString csyCode, quint64 grfId, IlwisTypes tp = itRASTER, IlwisTypes extTypes = itUNKNOWN, quint64 sz = i64UNDEF, int layerindex = iUNDEF, bool isExtendedType = true);
    quint64 addCsy(GdalHandle* handle, const QString &path, const QUrl &url, QString& code, bool message=true);
    QString dimensions(GdalHandle* handle, bool &is3d, int layerindex=iUNDEF) const;
    std::map<QString, QString> kvp2Map(char **kvplist);
    int handleComplexDataSet(void *handle);
    quint64 numbertype2domainid(const QString &numbertype) const;
    quint64 findSize(const QFileInfo &inf);
    int layerCount(GdalHandle* handle, bool& isColorRaster);
    quint64 caseWithurl(const QStringList &parts, Size<> &sz, QString &shortname);
    quint64 caseWithSquareBrackets(const QStringList &parts, Size<> &sz, QString &shortnames);
    Size<> getSize(const QStringList& szMembers);
    void addOffsetScale(void *handle, const int count, Ilwis::Resource &gdalitem);
};
}
}

#endif // GDALITEM_H
