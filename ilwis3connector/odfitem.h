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

#ifndef ODFITEM_H
#define ODFITEM_H

#include "inifile.h"

namespace Ilwis {
namespace Ilwis3{

class ODFItem : public Resource
{
public:
    ODFItem(const IniFile& file, std::unordered_map<QString, IniFile> *knownInis = 0);
    bool resolveNames(const QHash<QString, quint64>& names);
    std::vector<Resource> resolveNames();

    bool isMapList() const;
    const IniFile _ini;

private:
    /*!
     \brief  tries to find a id for the name found in the catalog

     All the names in the catalog have been scanned and they either have a resource already associated with them or
     they need to be associated with an existing resource. This can either be an resource in the catalog or a system object.
     Basically an object in the mastercatalog.

     \param names names found in this catalog
     \param unboundName name to be associated with an id
     \param returning id;

     \return true if the operation was succesfull. Error messages can be found on the issue stack.
    */
    bool setFileId(const QHash<QString, quint64> &names, const QString &unboundName, quint64 &fileid) const;

    QString findDomainName(const QString &path) const;
    IlwisTypes findDomainType(const QString &path) const;
    QString findCsyName(const QString &path) const;
    IlwisTypes findCsyType(const QString &path);
    QString findGrfName() const;
    QString findDatumName() const;
    QString findProjectionName() const;
    quint64 findSize() const;
    quint64 objectSize() const;
    quint64 partSize(const QUrl &file, const QString &section, const QString &key) const;
    QString findDimensions() const;
    QString stripExtension(const QString &name) const;
    static bool isSystemObject(const QString &name);
    QString cleanName(const QString&) const;
    Resource resolveName(const QString &name, IlwisTypes tp);


    std::unordered_map<QString, IniFile> *_knownOdfs=0;
    QString _grfname;
    QString _domname;
    QString _csyname;
    QString _datumName;
    QString _projectionName;
    QString _mplcontainer;
    bool _isMapList = false;

    const static QString systemObjectNames;

    bool getIni(IniFile &ini, const QString &localpath) const;
};
}
}

#endif // ODFITEM_H
