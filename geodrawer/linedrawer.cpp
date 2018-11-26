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
#include "linedrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

LineDrawer::LineDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : SimpleDrawer(name, parentDrawer, rootdrawer, IOOptions())
{

}

bool LineDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &, QOpenGLContext *openglContext)
{
    if ( prepType == DrawerInterface::ptGEOMETRY){

    }
    return true;
}

bool LineDrawer::draw(const IOOptions &options) const
{
    if (!isActive() || !isValid())
        return false;

    if ( _coords.size() == 0)
        return false;

    if ( containment() == BaseDrawer::cOUTSIDE)
        return false;

    return true;

}
