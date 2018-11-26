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

#ifndef ILWIS3CONNECTOR_H
#define ILWIS3CONNECTOR_H

#include <QScopedPointer>
#include "Ilwis3Connector_global.h"


namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace Ilwis3 {

class ILWIS3CONNECTORSHARED_EXPORT Ilwis3Connector : public Ilwis::IlwisObjectConnector {
public:
    Ilwis3Connector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    //static IlwisTypes type(const QString &name);
    static IlwisTypes ilwisType(const QString &name);
    static QString name2Code(const QString &name, const QString &type);
    static QString code2name(const QString &code, const QString &type);
    static QString outputNameFor(const IlwisObject *obj, bool isMulti, IlwisTypes type);

    QString provider() const;
    virtual bool storeBinaryData(IlwisObject* ) { return false; }
    virtual bool storeMetaData(IlwisObject*, const IOOptions& options=IOOptions() )  { return false; }
    bool isReadOnly() const;
    QString unquote(const QString &name) const;

    bool store(IlwisObject *, const IOOptions& options = IOOptions());

protected:
    bool willStore(const Ilwis::IlwisObject *obj) const;
    QUrl resolve(const Resource &resource) const;
    bool isSystemObject(const QString &filename);
    bool loadMetaData(IlwisObject* data,const IOOptions&);
    bool storeMetaData(const IlwisObject* data, IlwisTypes type) const;
    QString adjustPath(const QUrl& container, const QString& filename) const;
    QString unquote(const QString& name);


    QString ilwis3ClassName(IlwisTypes type) const;
    QString datum2Code(const QString& name, const QString& area) const;
    QString noExt(const QString& name);
    QString filename2FullPath(const QString &name, const Resource &owner=Resource()) const;
    QString writeCsy(IlwisObject *obj, const Ilwis::ICoordinateSystem &csy) const;

    static IniFile *makeIni(const Resource& res, IlwisTypes type);
    static QString suffix(IlwisTypes type) ;

    mutable ODF _odf;

    QUrl makeUrl(const QString &path, const QString &name=sUNDEF, IlwisTypes type=itUNKNOWN);
    static QString type2humanName(IlwisTypes tp);
};
}

}

#endif // ILWIS3CONNECTOR_H
