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

#include "kernel.h"
#include "drawerattributesetterfactory.h"

using namespace Ilwis;
using namespace Ui;

std::map<QString, DASCreator> DrawerAttributeSetterFactory::_creators;

DrawerAttributeSetterFactory::DrawerAttributeSetterFactory() : AbstractFactory("DrawerAttributeSetterFactory","UI", "Creates objects that are able to tell how a element is being drawn")
{

}

DrawerAttributeSetterFactory::~DrawerAttributeSetterFactory()
{

}

DrawerAttributeSetter *DrawerAttributeSetterFactory::registerDrawerAttributeSetter(const QString &name, DASCreator createFunc)
{
    auto iter = _creators.find(name.toLower());
    if ( iter == _creators.end()){
        _creators[name.toLower()] = createFunc;
    }
    return 0;
}
