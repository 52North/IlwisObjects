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

#ifndef DRAWERINTERFACEENUMS_H
#define DRAWERINTERFACEENUMS_H

#include <memory>
#include <QColor>
#include <GL/gl.h>
#include <QVector3D>
#include "ilwiscoreui_global.h"
#include <math.h>
#include "kernel.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "location.h"
#include "ilwisdata.h"
#include "representation.h"
#include "identityinterface.h"
//#include "complexdrawer.h"
#include "iooptions.h"

class QOpenGLContext;

namespace Ilwis {

namespace Geodrawer{

class ILWISCOREUISHARED_EXPORT DrawerInterface {
public:
    enum PreparationType{ptNONE=0,ptRENDER=1,ptGEOMETRY=2,ptINITOPENGL=4,ptUI=8,pt3D=16,ptANIMATION=32,
                         ptRESTORE=64,ptOFFSCREENSTART=128,ptOFFSCREENEND=256,
                         ptREDRAW=512,ptNEWCSY=1024,ptMVP=2048,ptSHADERS=4096, ptRESET=8192,ptALL=4294967295};
    enum ColorValueMeaning{cvmFRACTION, cvmTRUEVALUE};
    enum DrawerType{dtDONTCARE=0, dtPOST=1, dtPRE=2, dtMAIN=4, dtALL=0xFFFFFFFF};
};

}

}
#endif // DRAWERINTERFACEENUMS_H
