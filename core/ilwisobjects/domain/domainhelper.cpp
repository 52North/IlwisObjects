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
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "itemrange.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "identifierrange.h"
#include "juliantime.h"
#include "domainhelper.h"

using namespace Ilwis;

DomainHelper::DomainHelper()
{
}

IDomain DomainHelper::create(Range* range) {
    IDomain domain;
    switch(range->valueType()){
    case itINTEGER:
    case itFLOAT:
    case itDOUBLE:
    case itTIME:
    case itDATE:
    case itDATETIME:
        domain.set(new NumericDomain(static_cast<NumericRange *>(range)));break;
    case itTHEMATICITEM:
        domain.set(new ThematicDomain(range)); break;
    case itNAMEDITEM:
        domain.set(new NamedIdDomain(range)); break;

    };
    return domain;
}

IDomain DomainHelper::fromTimeInterval(const QString& beg, const QString& end, const Duration &step) {
    return DomainHelper::create(new TimeInterval(beg, end, step));
}

IDomain DomainHelper::fromNumericRange(double beg, double end, double step) {
    return DomainHelper::create(new NumericRange(beg, end, step));
}

