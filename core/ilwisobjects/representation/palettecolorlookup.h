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

#ifndef PALETTECOLORLOOKUP_H
#define PALETTECOLORLOOKUP_H

#include "colorlookup.h"
#include "boost/container/flat_map.hpp"

namespace Ilwis {
class KERNELSHARED_EXPORT PaletteColorLookUp : public ColorLookUp{

public:
    PaletteColorLookUp();
    PaletteColorLookUp(const QString &definition);
    PaletteColorLookUp(const Ilwis::IDomain &dom, const QString& rprCode= "");
    PaletteColorLookUp(boost::container::flat_map<quint32, QColor>& newcolors);
    QColor value2color(double index, const NumericRange& actualRange = NumericRange(), const NumericRange& stretchRange = NumericRange()) const;
    void setColor(double value, const QColor &clr);

    ColorLookUp *clone() const;
	void store(QDataStream& stream) const override;
	void load(QDataStream& stream) override;
	QString definition(const IDomain& dom, bool& hasChanged)  override;


private:
    boost::container::flat_map<quint32, QColor> _colors;
    bool _cyclic = true;
	QString _definition;

	void fromDefinition(const QString &definition, const Ilwis::IDomain &dom = IDomain());
	void reset(const IDomain& dom);

};
}

#endif // PALETTECOLORLOOKUP_H
