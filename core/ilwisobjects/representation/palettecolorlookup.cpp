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

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include <QColor>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "domain.h"
#include "itemdomain.h"
#include "domainitem.h"
#include "coloritem.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "interval.h"
#include "colorrange.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "palettecolorlookup.h"

using namespace Ilwis;
PaletteColorLookUp::PaletteColorLookUp(){

}

PaletteColorLookUp::PaletteColorLookUp(const QString &def){
    fromDefinition(def);
	_definition = def;
}

PaletteColorLookUp::PaletteColorLookUp(const IDomain &dom, const QString &rprCode)
{
    _cyclic = hasType(dom->valueType(), itINDEXEDITEM | itNAMEDITEM | itSTRING | itNAMEDITEM);
    InternalDatabaseConnection db;
    QString query = QString("Select * from representation where code='%1'").arg(rprCode != "" ? rprCode : "primarycolors" );
    if (db.exec(query)) {
        if ( db.next()){
            QSqlRecord rec = db.record();
            QString  definition = rec.field("definition").value().toString();
            fromDefinition(definition, dom);
        }
    }
}

PaletteColorLookUp::PaletteColorLookUp(boost::container::flat_map<quint32, QColor> &newcolors) : _colors(newcolors)
{

}

QColor PaletteColorLookUp::value2color(double index, const NumericRange &, const NumericRange &) const
{
    if (index != iUNDEF && _colors.size() > 0) {
        int localIndex = index;
        if (_cyclic) {
            localIndex = localIndex % _colors.size();
        }
        auto iter = _colors.find(localIndex);
        if (iter != _colors.end()) {

            QColor clr = (*iter).second;
            return clr;
        }
    }
    return QColor();
}

QString PaletteColorLookUp::definition(const IDomain& dom, bool& hasChanged)  {
	QString result;
	if (dom.isValid() && _colors.size() > 0) {
		if (hasType(dom->valueType(), itTHEMATICITEM | itNUMERICITEM | itTIMEITEM | itNAMEDITEM)) {
			IItemDomain itemdom = dom.as<ItemDomain<DomainItem>>();
			for (auto item : itemdom) {
				QString clr = _colors.at(item->raw()).name(QColor::HexArgb);
				if (result != "")
					result += "|";
				result += clr;

			}
		}
	}
	hasChanged = result != _definition;
	if (hasChanged)
		_definition = result;
	return result;
}

void PaletteColorLookUp::reset(const IDomain& dom) {
	fromDefinition(_definition, dom);
}
void PaletteColorLookUp::fromDefinition(const QString &definition, const IDomain& dom){
    QStringList parts = definition.split("|");
    if ( dom.isValid()){
        if ( hasType(dom->valueType(), itTHEMATICITEM|itNUMERICITEM|itTIMEITEM | itNAMEDITEM)){
            IItemDomain itemdom = dom.as<ItemDomain<DomainItem>>();
            int index = 0;
            for(auto item : itemdom){
                QString part = parts[index % parts.size()];
				QColor clr = string2color(part);
                _colors[(quint32)item->raw()] = clr;
                ++index;
            }
			_definition = definition;
            return;
        }
    }
   // int index = 0;
    _cyclic = true;
 /*  for( QString part : parts){
        QColor color = string2color(part);
        _colors[index - 1] = color;
        ++index;
    }*/
}

void PaletteColorLookUp::store(QDataStream& stream) const {
	stream << (quint32)_colors.size();
	for (auto item : _colors) {
		stream << item.first;
		stream << item.second;
	}
	stream << _definition;
	stream << _cyclic;
}

void PaletteColorLookUp::load(QDataStream& stream) {
	quint32 n;
	stream >> n;
	for (int i = 0; i < n; ++i) {

		quint32 raw;
		QColor clr;
		stream >> raw;
		stream >> clr;
		_colors[raw] = clr;
	}
	stream >> _definition;
	stream >> _cyclic;
}

ColorLookUp *PaletteColorLookUp::clone() const{
    PaletteColorLookUp *newpalette = new PaletteColorLookUp();
    newpalette->_colors = _colors;
    newpalette->_cyclic = _cyclic;
	newpalette->_definition = _definition;
    return newpalette;
}

void PaletteColorLookUp::setColor(double value, const QColor &clr)
{
    _colors[(quint32)value] = clr;
}

QString PaletteColorLookUp::definition() const {
	return _definition;
}
