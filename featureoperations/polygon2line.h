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

#ifndef POLYGON2LINE_H
#define POLYGON2LINE_H


namespace Ilwis {
namespace FeatureOperations {

class Polygon2Line : public OperationImplementation
{
public:
    Polygon2Line();
    ~Polygon2Line();
    Polygon2Line(quint64 metaid, const Ilwis::OperationExpression &expr);

    bool execute(ExecutionContext *ctx,SymbolTable& symTable);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    Ilwis::OperationImplementation::State prepare(ExecutionContext *ctx, const SymbolTable &);

    static quint64 createMetadata();

private:
    IFeatureCoverage _outputfeatures;
    IFeatureCoverage _inputfeatures;
    bool _singleId;
    NEW_OPERATION(Polygon2Line);
    void extractBoundary(const Ilwis::UPGeometry &g, quint32 &record);
};
}
}

#endif // POLYGON2LINE_H
