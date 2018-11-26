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

#ifndef DRAWERATTRIBUTESETTERFACTORY_H
#define DRAWERATTRIBUTESETTERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Ui {

class DrawerAttributeSetter;

typedef std::function<DrawerAttributeSetter* (const IOOptions &options)> DASCreator;

class ILWISCOREUISHARED_EXPORT DrawerAttributeSetterFactory : public AbstractFactory
{
public:
    DrawerAttributeSetterFactory();
    ~DrawerAttributeSetterFactory();

    static DrawerAttributeSetter * registerDrawerAttributeSetter(const QString& name, DASCreator createFunc);

    template<class T=DrawerAttributeSetter> static T *create(const QString name, const IOOptions &options)
    {
       auto iter = _creators.find(name.toLower());
       if ( iter == _creators.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second(options));
    }

private:
    static std::map<QString, DASCreator> _creators;
};
}
}

#endif // DRAWERATTRIBUTESETTERFACTORY_H
