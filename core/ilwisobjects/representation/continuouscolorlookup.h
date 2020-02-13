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
class ContinuousColorRange;

class KERNELSHARED_EXPORT ContinuousColorLookup : public ColorLookUp
{
	struct ValueRange {
		double _first = rUNDEF;
		double _last = rUNDEF;
		double _reversed = false;
		bool overlaps(const ValueRange& rng) const{
			if (_last == rng._last && _first == rng._first)
				return true;

			bool b1 = _last > rng._first && _last < rng._last;
			bool b2 = _first <= rng._first && _last >= rng._last;
			bool b3 = _first >= rng._first && rng._first < rng._last;
			return b1 || b2 || b3;
			//return _last < rng._first || (_first > rng._first && _last < rng._last) || (_first < rng._first && _last > rng._last);
		}

	};
public:
		enum StretchMethod{smLINEARA, smLINEARB, smLOGARITHMIC};
        ContinuousColorLookup();
        ContinuousColorLookup(const QString& definition, const QString& mode);
        ContinuousColorLookup(const IDomain&, const QString &rprCode="");
        QColor value2color(double value, const Ilwis::NumericRange &actualRange = NumericRange(), const Ilwis::NumericRange &stretchRange = NumericRange()) const;
        void addGroup(const ValueRange& range, const ContinuousColorRange& colorrange);
		void addGroup(const NumericRange& range, const ContinuousColorRange& colorrange);
		void addException(const NumericRange& range, const QColor& clr, bool clear=true);
		void deleteException(const NumericRange& range);
        void setColor(double value, const QColor& clr) ;
        ColorLookUp *clone() const;
		void store(QDataStream& stream) const override;
		void load(QDataStream& stream) override;
		QString definition(const IDomain& dom, bool& hasChanged)  override;
		QString definition() const override;
		NumericRange numericRange() const;
		void steps(int st);
		int steps() const;
		void stretchMethod(ContinuousColorLookup::StretchMethod yesno);
		StretchMethod stretchMethod() const;
		void setBoundMapping(int idx, double original, double mapped);
		std::pair<double, double> boundMapping(int idx);

		static double value2Color4BoundsMapping(double value, const NumericRange& stretchRange, const std::vector<std::pair<double, double>>& boundsMapping);
		

private:
		std::vector < std::pair< ValueRange, ContinuousColorRange>> _colorranges;
		std::vector< std::pair< ValueRange, QColor>> _exceptions;
		std::vector<std::pair<double, double>> _boundsMappings; // used in smLINEARB
        NumericRange _numericRange;
        double _step = 0;
		StretchMethod _stretchMethod = smLINEARA;
		bool _relative = true;
		bool _oldStyle = false;


		void fromDefinition(const QString& def, const IDomain& dom = IDomain()); 
		void reset(const IDomain& dom);

};
}

#endif // CONTINUOUSCOLORLOOKUP_H
