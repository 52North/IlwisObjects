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

#ifndef CONTINUOUSCOLORLOOKUP_H
#define CONTINUOUSCOLORLOOKUP_H

#include "colorlookup.h"

namespace Ilwis {
class Domain;
typedef IlwisData<Domain> IDomain;

class KERNELSHARED_EXPORT ContinuousColorLookup : public ColorLookUp
{
	struct ValueRange {
		double _first = rUNDEF;
		double _last = rUNDEF;
		double _reversed = false;
		bool overlaps(const ValueRange& rng) {
			return _last < rng._first || (_first > rng._first && _last < rng._last) || (_first < rng._first && _last > rng._last);
		}
	};
public:
        ContinuousColorLookup();
        ContinuousColorLookup(const QString& definition);
        ContinuousColorLookup(const IDomain&, const QString &rprCode="");
        QColor value2color(double value, const Ilwis::NumericRange &actualRange = NumericRange(), const Ilwis::NumericRange &stretchRange = NumericRange()) const;
        void addGroup(const ValueRange& range, const ContinuousColorRange& colorrange);
        void setColor(double value, const QColor& clr) ;
        ColorLookUp *clone() const;

private:

        std::vector<ValueRange> _groups;
        std::vector<ContinuousColorRange> _colorranges;
        NumericRange _numericRange;
        double _step = 0;
        bool _linear = true;

        void fromDefinition(const QString& def);

};
}

#endif // CONTINUOUSCOLORLOOKUP_H
