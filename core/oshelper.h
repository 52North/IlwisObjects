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

#ifndef OSHELPER_H
#define OSHELPER_H

namespace Ilwis {
class KERNELSHARED_EXPORT OSHelper
{
public:
    OSHelper();

    static QString neutralizeFileName(const QString& somelocation, bool allNames=false) ;
    static bool isAbsolute(const QString& fragment);
    static bool isFileName(const QString& filename);
    static QString operatingSystem();
    static QString createFileUrlFromParts(const QString &left, const QString &right);
    static QString ensureUniqueFilename(QString fileFullPath);
};
}

#endif // OSHELPER_H
