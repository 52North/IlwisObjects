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
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "range.h"
#include "numericrange.h"
#include "itemrange.h"
#include "identifierrange.h"
#include "interval.h"
#include "intervalrange.h"
#include "juliantime.h"
#include "geometries.h"
#include "box.h"
#include "colorrange.h"

using namespace Ilwis;

Range::Range()
{
}

bool Range::isContinuous() const
{
    return _interpolation.size() != 0;
}

QString Range::interpolation() const
{
    return _interpolation;
}

void Range::interpolation(const QString &ip)
{
    _interpolation = ip;
}

Range *Range::create(IlwisTypes types) {

    switch (types){
    case itNAMEDITEM:
        return new NamedIdentifierRange();
    case itTHEMATICITEM:
        return new ThematicRange();
    case itINDEXEDITEM:
        return new IndexedIdentifierRange();
    case itNUMERICITEM:
        return new IntervalRange();
    case itTIME:
        return new TimeInterval();
    case itENVELOPE:
        return new Envelope();
    case itBOUNDINGBOX:
        return new BoundingBox();
    case itCOLOR:
        return new ContinuousColorRange();
    case itPALETTECOLOR:
        return new ColorPalette();
	case itDATETIME:
		return new TimeInterval();
    default:
        if ( hasType(types, itNUMBER)){
            return new NumericRange();
        }
    }
    return 0;
}
