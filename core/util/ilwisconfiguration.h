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

#ifndef ILWISCONFIGURATION_H
#define ILWISCONFIGURATION_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace Ilwis {
class KERNELSHARED_EXPORT IlwisConfiguration
{
public:
    IlwisConfiguration();
    bool prepare(const QString& configlocation);

    template<typename ValueType> ValueType operator ()(const QString& key, const ValueType& defaultValue){
        QString tempkey = key;
        tempkey.replace("/",".");
        ValueType v = _configuration.get(tempkey.toStdString(),defaultValue);
        return v;
    }
    void eraseChildren(const QString& key);
    void store(const QString &location=sUNDEF);
    void addValue(const QString &key, const QString &value);
    void putValue(const QString &key, const QString &value);
private:
   boost::property_tree::ptree _configuration;
   QString _configLocation;
   bool _modified = false;
};

template<> inline QString IlwisConfiguration::operator ()(const QString& key, const QString& defaultValue){
    QString tempkey = key;
    tempkey.replace("/",".");
    std::string v = _configuration.get(tempkey.toStdString(),defaultValue.toStdString());
    return QString::fromStdString(v);
}
}

#endif // ILWISCONFIGURATION_H
