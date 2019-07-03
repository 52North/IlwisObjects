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

#ifndef VERSION_H
#define VERSION_H

#include "kernel_global.h"
#include "module.h"


namespace Ilwis {

class KERNELSHARED_EXPORT Version {
public:
    Version();
    enum BinaryVersion{ bvFORMAT14, bvFORMAT20, bvFORMAT30, bvFORMATFOREIGN, bvUNKNOWN, bvPOLYGONFORMAT37, bvILWISOBJECTS1};

    bool supportsBinaryVersion(Ilwis::Version::BinaryVersion version, bool throwError = true);
    bool supportsODFVersion(const QString& version, bool throwError=true);
    void addBinaryVersion(Ilwis::Version::BinaryVersion v);
    void addODFVersion(const QString& v);
    bool supportsModuleInterfaceVersion(const QString& inf, const QString& module,bool throwError=true) const;
    void addModuleInterfaceVersion(const QString& inf);
    const static QString interfaceVersion40;
	const static QString interfaceVersion41;
    const static QString IlwisShortName;
    const static QString cacheVersion;
	const static QString adjustmentVersion;

    QString buildType() const;
    QString verionNumber() const;
    QString buildNumber() const;
    QString triangulationVersionNumber() const;
private:
    static QString binaryVersionNumberToString(Ilwis::Version::BinaryVersion v) ;
    QVector<QString> supportedODFVersions;
    QVector<BinaryVersion> binaryVersions;
    QVector<QString> moduleVersions;

};

}

#endif // VERSION_H
