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

#include <QString>
#include <QVariant>
#include <map>
#include "iooptions.h"

using namespace Ilwis;

bool IOOptions::isEmpty() const
{
    return size() == 0;
}

IOOptions::IOOptions(const QVariantMap& other) {
	for (auto iter = other.begin(); iter != other.end(); ++iter) {
		addOption({ iter.key(), iter.value() });
	}
}

IOOptions &IOOptions::operator<<(const IOOptions::Option &option)
{
    return addOption(option._key, option._value);
}

IOOptions &IOOptions::addOption(const QPair<QString, QVariant>& item)
{
    return addOption(item.first, item.second);
}

IOOptions &IOOptions::addOption(const QString &key, const QVariant &value)
{
    if (!value.isValid()){
        auto iter = find(key);
        if ( iter != end())
            erase(iter);
    }
    else if ( key != "?")
        (*this)[key] = value;
    return *this;
}
