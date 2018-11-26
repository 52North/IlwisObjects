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

#ifndef SHAPELOOKUP_H
#define SHAPELOOKUP_H

#include "boost/container/flat_map.hpp"

//TODO this is meant to be a more or less a port of the IVG stuff of Ilwis 3,
namespace Ilwis {
class KERNELSHARED_EXPORT Shape{

public:
    enum ShapeType{sRECTANGLE, sCIRCLE,sELLIPSE,sLINE,sPOLYLINE,sPOLYGON,sPATH, sCOMPOUND, sUNKNOWN};

    Shape();
private:
};

class ShapeLookUp
{
public:
    ShapeLookUp();
    ~ShapeLookUp();

    Shape shape(Raw raw);

    ShapeLookUp* clone() const;

private:
    boost::container::flat_map<Raw, Shape> _shapes;
};
}

#endif // SHAPELOOKUP_H
