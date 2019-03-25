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
#include "domain.h"
#include "datadefinition.h"
#include "combinationmatrix.h"

using namespace Ilwis;

CombinationMatrix::CombinationMatrix()
{
}

CombinationMatrix::CombinationMatrix(const Resource &resource) : IlwisObject(resource)
{
    _axisDef.resize(2);
    _axisValues.resize(2);
}

IlwisTypes CombinationMatrix::ilwisType() const
{
    return itCOMBINATIONMATRIX;
}

DataDefinition CombinationMatrix::axisDefinition(CombinationMatrix::Axis axis) const
{
    return _axisDef[axis];
}

void CombinationMatrix::axisDefinition(CombinationMatrix::Axis axis, const DataDefinition &coldef)
{
    _axisDef[axis] = coldef;
    _combos = std::vector<std::vector<double>>();
}

DataDefinition CombinationMatrix::combinationDef() const
{
    return _comboDef;
}

void CombinationMatrix::combinationDef(const DataDefinition &def)
{
    _comboDef = def;
   _combos = std::vector<std::vector<double>>();
}

QString CombinationMatrix::comboAsString(const QString &xValue, const QString &yValue) const
{
    double x = _axisDef[aXAXIS].domain()->impliedValue(xValue).toDouble();
    double y = _axisDef[aYAXIS].domain()->impliedValue(yValue).toDouble();

    return comboAsString(x,y);
}

QString CombinationMatrix::comboAsString(int x, int y) const
{
    if ( x < _axisValues[aXAXIS].size() &&  y < _axisValues[aYAXIS].size()){
        if ( _comboDef.domain()->ilwisType() == itNUMERICDOMAIN){
            double v = _combos[x][y] ;
            if ( v== rUNDEF)
                return sUNDEF;
            return QString::number(v );

        }
        if ( _comboDef.domain()->ilwisType() == itITEMDOMAIN){
            return _comboDef.domain()->impliedValue(_combos[x][y] ).toString();
        }
    }
    return sUNDEF;
}

double CombinationMatrix::combo(const QString &xValue, const QString &yValue)
{
    double x = _axisDef[aXAXIS].domain()->impliedValue(xValue).toDouble();
    double y = _axisDef[aYAXIS].domain()->impliedValue(yValue).toDouble();

    return combo(x,y);
}

double CombinationMatrix::combo(int x, int y) const
{
    if ( (x >= 0 && x < _axisValues[aXAXIS].size()) &&  (y>=0 && y < _axisValues[aYAXIS].size())){
            return _combos[x][y] ;
    }
    return rUNDEF;
}

void CombinationMatrix::combo(int x, int y, double v)
{
    if ( _combos.size() == 0 ){
        _combos.resize(_axisValues[aXAXIS].size());
        for(auto& data : _combos){
            data.resize(_axisValues[aYAXIS].size(),rUNDEF);
        }
    }
    if ( v == rUNDEF)
        return;
    if ( x < _axisValues[aXAXIS].size() &&  y < _axisValues[aYAXIS].size()){
            QVariant val = _comboDef.domain() ->impliedValue(v);
            if ( val.isValid())
                _combos[x][y]  = v;
    }
}

QString CombinationMatrix::axisValue(CombinationMatrix::Axis axis, int index)
{
    if ( index < _axisValues[axis].size()){
        return _axisValues[axis][index];
    }
    return sUNDEF;
}

void CombinationMatrix::axisValues(CombinationMatrix::Axis axis, const std::vector<QString> &values)
{
    DataDefinition def = _axisDef[axis];
    for ( auto value : values){
        if ( def.domain()->contains(value) !=  Domain::cSELF){
            kernel()->issues()->log(TR("Value is not valid for axis:" + value)) ;
            return;
        }
    }

    _axisValues[axis] = values;
}

double CombinationMatrix::axisValueCount(CombinationMatrix::Axis axis) const
{
    return _axisValues[axis].size();
}

IlwisObject *CombinationMatrix::clone()
{
        CombinationMatrix *matrix = new CombinationMatrix();
        copyTo(matrix);

        return matrix;
}

void CombinationMatrix::copyTo(IlwisObject *obj)
{
    IlwisObject::copyTo(obj);
    CombinationMatrix *btable = static_cast<CombinationMatrix *>(obj);
    btable->_axisDef = _axisDef;
    btable->_axisValues = _axisValues;
    btable->_comboDef = _comboDef;
    btable->_combos = _combos;


}

