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

#include "drawercolorsets.h"


using namespace Ilwis;
using namespace Geodrawer;


DrawerColorsets::DrawerColorsets()
{
    QColor area(80,80,0);
    area.setAlphaF(0.15);

    QColor border(0,0,0);
    border.setAlphaF(1);
    _colorsets.insert("basic",Colorset { area, border } );

    QColor area2(0,0,0);
    area2.setAlphaF(0.0);
    QColor border2(255,255,0); 
    border2.setAlphaF(0);
    _colorsets.insert("colorless",Colorset { area2, border2 } );
}


Colorset DrawerColorsets::GetColorset(const QString &colorset) const {

    return _colorsets[colorset]; 
}

Colorset DrawerColorsets::operator [](const QString &key) const
{
    return GetColorset(key);
}
