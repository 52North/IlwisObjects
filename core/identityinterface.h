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

#ifndef IDENTITYINTERFACE_H
#define IDENTITYINTERFACE_H

#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT IdentityInterface {
public:
    friend class Resource;

    IdentityInterface();
    virtual ~IdentityInterface();
    virtual void code(const QString& code) = 0;
    virtual QString code() const = 0;
    virtual quint64 id() const = 0;
    virtual QString name() const = 0;
    virtual void name(const QString& n) = 0;
    virtual QString description() const = 0;
    virtual void setDescription(const QString& desc) = 0;
};
}



#endif // IDENTITYINTERFACE_H
