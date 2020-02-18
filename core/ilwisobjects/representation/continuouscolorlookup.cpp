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
		auto sm = _stretchMethod;
		if (sm == smLINEARB) {
			// check if its valid else default to smLINEARA
			sm = smLINEARA;
			if (_boundsMappings.size() == 2) {
				if (_boundsMappings[0].first < _boundsMappings[1].first && 
					_boundsMappings[0].second < _boundsMappings[1].second) {
					sm = smLINEARB;
				}
			}
		}
        if ( sm == smLINEARA){
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
		else if (sm == smLINEARB) {
			value = value2Color4BoundsMapping(value, actualRange, _boundsMappings);
		}
    }
	if (_relative)
		value = min(1.0, max(0.0, (value - actualRange.min()) / actualRange.distance())); // scale it between 0..1 
	else
		value = min(actualRange.max(), max(actualRange.min(), (value - actualRange.min()))); // scale it between 0..1

	for (const auto& group : _exceptions) {
		if (value >= group.first._first && value <= group.first._last) {
			return group.second;
		}
	}
    for(int i = 0; i < _colorranges.size(); ++i){
		double delta = std::abs(_colorranges[i].first._last - _colorranges[i].first._first);
		double position = 0;
		if (_colorranges[i].first._reversed) {
			if (value < _colorranges[i].first._first && value >= _colorranges[i].first._last) {
				
				if (_step == 0) {
					position = (value - _colorranges[i].first._last) / delta;
				}
				else
					position = ((value - _colorranges[i].first._last) / _step) / ((delta / _step));

				position = 1.0 - position;
				return ContinuousColorRange::valueAt(position, &_colorranges[i].second);
			}
		} else if (value <= _colorranges[i].first._last) {
            double position = 0;
            if ( _step == 0){
                position = (value - _colorranges[i].first._first)/ delta;
            }else
                position = ((value - _colorranges[i].first._first)/ _step)/( (delta / _step));

            return ContinuousColorRange::valueAt(position,&_colorranges[i].second);
        }
    }
    return QColor();
}

void ContinuousColorLookup::addGroup(const ValueRange &range, const ContinuousColorRange &colorrange)
{
	if (_colorranges.size() == 0) {
		_colorranges.push_back(std::pair< ValueRange, ContinuousColorRange>(range, colorrange));
	}else if (!_colorranges.back().first.overlaps(range)) {
		_colorranges.push_back(std::pair< ValueRange, ContinuousColorRange>(range, colorrange));
	}
}

NumericRange ContinuousColorLookup::numericRange() const {
	NumericRange rng;
	for (auto& item : _colorranges) {
		rng += item.first._first;
		rng += item.first._last;
	}
	return rng;
}

void ContinuousColorLookup::setColor(double value, const QColor &clr)
{
    // TODO:
}

ColorLookUp *ContinuousColorLookup::clone() const
{
    ContinuousColorLookup *newlookup = new ContinuousColorLookup();
    newlookup->_colorranges = _colorranges;
    newlookup->_oldStyle = _oldStyle;
    newlookup->_numericRange = _numericRange;
    newlookup->_step = _step;
	newlookup->_definition = _definition;
	newlookup->_stretchMethod = _stretchMethod;

    return newlookup;
}

void ContinuousColorLookup::store(QDataStream& stream) const {
	stream << (quint32)_colorranges.size();
	for (auto item : _colorranges) {
		stream << item.first._first;
		stream << item.first._last;
		stream << item.first._reversed;
		item.second.store(stream);
	}
	_numericRange.store(stream);
	stream << _step;
	stream << _oldStyle;
	stream << _relative;
	stream << _definition;
	if (!_oldStyle) {
		stream << (int)_stretchMethod;
	}
}
void ContinuousColorLookup::load(QDataStream& stream) {
	quint32 n;
	stream >> n;
	_colorranges.resize(n);
	for (auto& vr : _colorranges) {
		stream >> vr.first._first;
		stream >> vr.first._last;
		stream >> vr.first._reversed;
		vr.second.load(stream);
	}
	_numericRange.load(stream);
	stream >> _step;
	stream >> _oldStyle;
	stream >> _relative;
	stream >> _definition;
	if (!_oldStyle) {
		int temp;
		stream >> temp;
		_stretchMethod = (StretchMethod)temp;
	}
}

QString ContinuousColorLookup::definition(const IDomain& dom, bool& hasChanged)  {
	QString def;
	for (auto crange : _colorranges) {
		if (def != "")
			def += ";";
		def += QString("%1:%2|%3|%4").arg(crange.first._first).arg(crange.first._last).arg(crange.second.limitColor1().name()).arg(crange.second.limitColor2().name());
	}
	hasChanged = def != _definition;
	if (hasChanged)
		_definition = def;
	return def;
}

void ContinuousColorLookup::reset(const IDomain& dom) {
}

void ContinuousColorLookup::fromDefinition(const QString &definition, const IDomain&)
{
	_colorranges.clear();
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
	_definition = definition;

}

void ContinuousColorLookup::addGroup(const NumericRange& range, const ContinuousColorRange& colorrange) {
	ValueRange vr;
	vr._first = range.min();
	vr._last = range.max();
	addGroup(vr, colorrange);
}

void ContinuousColorLookup::deleteException(const NumericRange& range) {

	if (!range.isValid())
		return;
	ValueRange vr{ range.min(), range.max() };
	for (auto iter = _exceptions.begin(); iter != _exceptions.end(); ++iter) {
		auto group = iter->first;
		if (group._first == range.min() && group._last == range.max()) {
			_exceptions.erase(iter);
			return;
		}
	}
}

void ContinuousColorLookup::addException(const NumericRange& range, const QColor& clr, bool clear) {

	if (clear)
		_exceptions.clear();
	if (!range.isValid())
		return;
	ValueRange vr{ range.min(), range.max() };
	for (auto& group : _exceptions) {
		if (group.first.overlaps(vr)) {
			group.second = clr;
			return;
		}
	}
	_exceptions.push_back(std::pair<ValueRange, QColor>(vr, clr));
}

void ContinuousColorLookup::relative(bool yesno) {
	_relative = yesno;

}

bool ContinuousColorLookup::relative() const {
	return _relative;
}

void ContinuousColorLookup::steps(int st) {
	if (st > 0) {
		_step = st;
	}
}

int ContinuousColorLookup::steps() const {
	return _step;
}

void ContinuousColorLookup::stretchMethod(ContinuousColorLookup::StretchMethod m) {
	_stretchMethod = m;
}
ContinuousColorLookup::StretchMethod ContinuousColorLookup::stretchMethod() const {
	return _stretchMethod;
}

QString ContinuousColorLookup::definition() const {
	return _definition;
}

void ContinuousColorLookup::setBoundMapping(int idx, double original, double mapped) {
	if (idx >= _boundsMappings.size()) {
		_boundsMappings.resize(idx + 1);
	}
	_boundsMappings[idx] = { original, mapped };
}
std::pair<double, double> ContinuousColorLookup::boundMapping(int idx) {
	if (idx < _boundsMappings.size())
		return _boundsMappings[idx];

	return { rUNDEF, rUNDEF };
}

double ContinuousColorLookup::value2Color4BoundsMapping(double value, const NumericRange& actualRange, const std::vector<std::pair<double, double>>& boundsMappings) {
	double scale = 1.0;
	if (value <= boundsMappings[0].first) {
		double unscaledDistance = boundsMappings[0].first - actualRange.min();
		double scaledDistance = boundsMappings[0].second - actualRange.min();
		if (unscaledDistance != 0)
			scale = scaledDistance / unscaledDistance;
		double d = (value - actualRange.min());
		value = d * scale;
	}
	else if (value >= boundsMappings[1].first) {
		double unscaledDistance = actualRange.max() - boundsMappings[1].first;
		double scaledDistance = actualRange.max() - boundsMappings[1].second;
		if (unscaledDistance != 0)
			scale = scaledDistance / unscaledDistance;
		value = (value - boundsMappings[1].first) * scale + boundsMappings[1].second;
	}
	else {
		double unscaledDistance = boundsMappings[1].first - boundsMappings[0].first;
		double scaledDistance = boundsMappings[1].second - boundsMappings[0].second;
		if (unscaledDistance != 0)
			scale = scaledDistance / unscaledDistance;
		double d = (value - boundsMappings[0].first);
		value =  d * scale + boundsMappings[0].second;
	}
	return value;
}