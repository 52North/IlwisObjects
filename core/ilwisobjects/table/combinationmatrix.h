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

#ifndef COMBINATIONMATRIX_H
#define COMBINATIONMATRIX_H

#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT CombinationMatrix : public IlwisObject
{
public:
    enum Axis {aXAXIS, aYAXIS};
    CombinationMatrix();

    CombinationMatrix(const Resource &resource);
    IlwisTypes ilwisType() const;

    DataDefinition axisDefinition( Axis axis) const;
    void axisDefinition(Axis axis,const DataDefinition& coldef);
    DataDefinition combinationDef() const;
    void combinationDef(const DataDefinition& def);
    QString comboAsString(const QString& xValue, const QString& yValue) const;
    QString comboAsString(int x, int y) const;
    double combo(const QString& xValue, const QString& yValue);
    double combo(int x, int y) const;
    void combo(int x, int y, double v);
    QString axisValue(Axis axis,int index);
    void axisValues(Axis axis, const std::vector<QString>& values);
    double axisValueCount(Axis axis) const;

    IlwisObject *clone();
private:
    std::vector<DataDefinition> _axisDef;
    DataDefinition _comboDef;
    std::vector<std::vector<QString>> _axisValues;

    std::vector<std::vector<double>> _combos;
    void copyTo(IlwisObject *obj);
};

typedef Ilwis::IlwisData<Ilwis::CombinationMatrix> ICombinationMatrix;

}
Q_DECLARE_METATYPE(Ilwis::ICombinationMatrix);


#endif // COMBINATIONMATRIX_H
