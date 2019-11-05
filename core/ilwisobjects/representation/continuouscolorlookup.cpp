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
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "itemrange.h"
#include "colorrange.h"
#include "numericrange.h"
#include "continuouscolorlookup.h"

using namespace Ilwis;



ContinuousColorLookup::ContinuousColorLookup()
{

}

ContinuousColorLookup::ContinuousColorLookup(const QString &definition, const QString& mode) : _relative(mode == "relative")
{
    fromDefinition(definition);
}

ContinuousColorLookup::ContinuousColorLookup(const IDomain &, const QString& rprCode)
{
    InternalDatabaseConnection db;
    QString query = QString("Select * from representation where code='%1'").arg(rprCode != "" ? rprCode : "pseudo");
    if (db.exec(query)) {
        if ( db.next()){
            QSqlRecord rec = db.record();
            QString  definition = rec.field("definition").value().toString();
            fromDefinition(definition);
        }
    }


}

QColor ContinuousColorLookup::value2color(double value, const NumericRange& actualRange, const NumericRange& stretchRange) const
{
    if ( value == rUNDEF)
        return QColor("transparent");

    if ( stretchRange.isValid())  {
        if ( _linear){
            if ( value < stretchRange.center()){
                double stretchFraction = (value - stretchRange.min())/ stretchRange.distance();
                value = actualRange.min() + stretchFraction * actualRange.distance();
            }else{
                if ( value >= stretchRange.center()){
                    double stretchFraction = (stretchRange.max() - value)/ stretchRange.distance();
                    value = actualRange.max() - stretchFraction * actualRange.distance(); 
                }
            }
        }
    }
	if (_relative)
		value = min(1.0, max(0.0, (value - actualRange.min()) / actualRange.distance())); // scale it between 0..1 
	else
		value = min(actualRange.max(), max(actualRange.min(), (value - actualRange.min()))); // scale it between 0..1
    for(int i = 0; i < _groups.size(); ++i){
		double delta = std::abs(_groups[i]._last - _groups[i]._first);
		double position = 0;
		if (_groups[i]._reversed) {
			if (value < _groups[i]._first && value >= _groups[i]._last) { 
				
				if (_step == 0) {
					position = (value - _groups[i]._last) / delta;
				}
				else
					position = ((quint32)(value - _groups[i]._last) / _step) / ((quint32)(delta / _step));

				position = 1.0 - position;
				return ContinuousColorRange::valueAt(position, &_colorranges[i]);
			}
		} else if (value <= _groups[i]._last) {
            double position = 0;
            if ( _step == 0){
                position = (value - _groups[i]._first)/ delta;
            }else
                position = ((quint32)(value - _groups[i]._first)/ _step)/( (quint32)(delta / _step));

            return ContinuousColorRange::valueAt(position,&_colorranges[i]);
        }
    }
    return QColor();
}

void ContinuousColorLookup::addGroup(const ValueRange &range, const ContinuousColorRange &colorrange)
{
	if (_groups.size() == 0) {
		_groups.push_back(range);
		_colorranges.push_back(colorrange);
	}else if (!_groups.back().overlaps(range)) {
		_colorranges.push_back(colorrange);
		_groups.push_back(range);
	}
}

void ContinuousColorLookup::setColor(double value, const QColor &clr)
{
    // TODO:
}

ColorLookUp *ContinuousColorLookup::clone() const
{
    ContinuousColorLookup *newlookup = new ContinuousColorLookup();
    newlookup->_colorranges = _colorranges;
    newlookup->_groups = _groups;
    newlookup->_linear = _linear;
    newlookup->_numericRange = _numericRange;
    newlookup->_step = _step;

    return newlookup;
}

void ContinuousColorLookup::store(QDataStream& stream) const {
	stream << (quint32)_groups.size();
	for (auto item : _groups) {
		stream << item._first;
		stream << item._last;
		stream << item._reversed;
	}
	stream << (quint32)_colorranges.size();
	for (auto item : _colorranges) {
		item.store(stream);
	}
	_numericRange.store(stream);
	stream << _step;
	stream << _linear;
	stream << _relative;
}
void ContinuousColorLookup::load(QDataStream& stream) {
	quint32 n;
	stream >> n;
	_groups.resize(n);
	for (auto& vr : _groups) {
		stream >> vr._first;
		stream >> vr._last;
		stream >> vr._reversed;
	}
	stream >> n;
	_colorranges.resize(n);
	for(auto& item : _colorranges)
	{
		item.load(stream);
	}
	_numericRange.load(stream);
	stream >> _step;
	stream >> _linear;
	stream >> _relative;
}

QString ContinuousColorLookup::definition(const IDomain& dom, bool& hasChanged)  {
	hasChanged = false;
	return "";
}

void ContinuousColorLookup::reset(const IDomain& dom) {
}

void ContinuousColorLookup::fromDefinition(const QString &definition, const IDomain&)
{
    QStringList parts = definition.split(";");
    for( QString group : parts){
        QStringList groupdef = group.split("|");
        if ( groupdef.size() != 3) {
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        QStringList limits = groupdef[0].split(":");
        if ( limits.size() != 2){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        bool ok1, ok2;
		ValueRange vr;
		vr._first = limits[0].toDouble(&ok1);
		vr._last = limits[1].toDouble(&ok2);
        if ( !(ok1 && ok2)){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
		vr._reversed = vr._last < vr._first;
        QColor color1 = string2color(groupdef[1]);
        if ( !(color1.isValid())){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }


        QColor color2 = string2color(groupdef[2]);
        if ( !(color2.isValid())){
            ERROR2(ERR_ILLEGAL_VALUE_2,TR("Representation definition"), definition);
            return;
        }
        ContinuousColorRange colorrange(color1, color2);
        addGroup(vr,colorrange);

    }

}
