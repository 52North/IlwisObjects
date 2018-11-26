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

#ifndef DATAFORMAT_H
#define DATAFORMAT_H
#include "kernel_global.h"

class QSqlQuery;

namespace Ilwis {
class KERNELSHARED_EXPORT DataFormat
{
public:
    enum FormatProperties{fpCODE, fpNAME, fpDESCRIPTION, fpEXTENSION, fpCONTAINER, fpDATATYPE, fpCONNECTOR, fpREADWRITE, fpEXTENDEDTYPE, fpPARTS};
    DataFormat();
    DataFormat(const QString& code, const QString connector = sUNDEF);
    DataFormat(const QString &connector, const QString& code, const QString& longname,
               const QString& extensions, const QString& access, IlwisTypes datatypes, const QString& description=sUNDEF, const QString& parts="");
    QVariant property(FormatProperties prop) const;
    bool isValid() const;
    bool store();

    static QVariantList getFormatProperties(FormatProperties, IlwisTypes types, QString connector=sUNDEF, QString code=sUNDEF);
    static bool supports(FormatProperties fp, IlwisTypes tp, const QVariant& prop, const QString& connector=sUNDEF);
    static bool setFormatInfo(const QString &path, const QString connector);
    static std::multimap<QString, DataFormat> getSelectedBy(FormatProperties prop, const QString &selection);

private:
    QVariant set(const QVariant& original) const;
    std::map<FormatProperties, QVariant> _properties;
    bool _isValid = false;

    void setProps(Ilwis::InternalDatabaseConnection &db, const QString &code);
};
}

#endif // DATAFORMAT_H
