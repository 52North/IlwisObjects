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

#ifndef UnaryMathRasterAndNumberOPERATIONS_H
#define UnaryMathRasterAndNumberOPERATIONS_H

namespace Ilwis {
namespace BaseOperations{
class SineCoverage : public UnaryMathRasterAndNumber
{
public:
    SineCoverage() {}
    SineCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SineCoverage);
};

class CosineCoverage : public UnaryMathRasterAndNumber
{
public:
    CosineCoverage() {}
    CosineCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(CosineCoverage);
};

class TangentCoverage : public UnaryMathRasterAndNumber
{
public:
    TangentCoverage() {}
    TangentCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TangentCoverage);
};

class ArcsineCoverage : public UnaryMathRasterAndNumber
{
public:
    ArcsineCoverage() {}
    ArcsineCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(ArcsineCoverage);
};

class ArccosineCoverage : public UnaryMathRasterAndNumber
{
public:
    ArccosineCoverage() {}
    ArccosineCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();


    NEW_OPERATION(ArccosineCoverage);
};

class ArcTangentCoverage : public UnaryMathRasterAndNumber
{
public:
    ArcTangentCoverage() {}
    ArcTangentCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(ArcTangentCoverage);
};

class Log10Coverage : public UnaryMathRasterAndNumber
{
public:
    Log10Coverage() {}
    Log10Coverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Log10Coverage);
};

class SqrtCoverage : public UnaryMathRasterAndNumber
{
public:
    SqrtCoverage() {}
    SqrtCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SqrtCoverage);
};

class NaturalLogarithmCoverage : public UnaryMathRasterAndNumber
{
public:
    NaturalLogarithmCoverage() {}
    NaturalLogarithmCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(NaturalLogarithmCoverage);
};

class EulerExpCoverage : public UnaryMathRasterAndNumber
{
public:
    EulerExpCoverage() {}
    EulerExpCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(EulerExpCoverage);
};

class AbsCoverage : public UnaryMathRasterAndNumber
{
public:
    AbsCoverage() {}
    AbsCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(AbsCoverage);
};

class CeilCoverage : public UnaryMathRasterAndNumber
{
public:
    CeilCoverage() {}
    CeilCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();
    NumericRange *constructRangeFrom(const SPNumericRange &range);

    NEW_OPERATION(CeilCoverage);
};

class FloorCoverage : public UnaryMathRasterAndNumber
{
public:
    FloorCoverage() {}
    FloorCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(FloorCoverage);
    NumericRange *constructRangeFrom(const SPNumericRange &range);
};

class SignCoverage : public UnaryMathRasterAndNumber
{
public:
    SignCoverage() {}
    SignCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SignCoverage);
};

class CosineHCoverage : public UnaryMathRasterAndNumber
{
public:
    CosineHCoverage() {}
    CosineHCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(CosineHCoverage);
};

class SineHCoverage : public UnaryMathRasterAndNumber
{
public:
    SineHCoverage() {}
    SineHCoverage(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SineHCoverage);
};

class SineTable : public UnaryMathTable
{
public:
    SineTable() {}
    SineTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SineTable);
};

class CosineTable : public UnaryMathTable
{
public:
    CosineTable() {}
    CosineTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(CosineTable);
};

class TangentTable : public UnaryMathTable
{
public:
    TangentTable() {}
    TangentTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(TangentTable);
};

class ArcsineTable : public UnaryMathTable
{
public:
    ArcsineTable() {}
    ArcsineTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(ArcsineTable);
};

class ArccosineTable : public UnaryMathTable
{
public:
    ArccosineTable() {}
    ArccosineTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();


    NEW_OPERATION(ArccosineTable);
};

class ArcTangentTable : public UnaryMathTable
{
public:
    ArcTangentTable() {}
    ArcTangentTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(ArcTangentTable);
};

class Log10Table : public UnaryMathTable
{
public:
    Log10Table() {}
    Log10Table(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(Log10Table);
};

class SqrtTable : public UnaryMathTable
{
public:
    SqrtTable() {}
    SqrtTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SqrtTable);
};

class NaturalLogarithmTable : public UnaryMathTable
{
public:
    NaturalLogarithmTable() {}
    NaturalLogarithmTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(NaturalLogarithmTable);
};

class EulerExpTable : public UnaryMathTable
{
public:
    EulerExpTable() {}
    EulerExpTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(EulerExpTable);
};

class AbsTable : public UnaryMathTable
{
public:
    AbsTable() {}
    AbsTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(AbsTable);
};

class CeilTable : public UnaryMathTable
{
public:
    CeilTable() {}
    CeilTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(CeilTable);
};

class FloorTable : public UnaryMathTable
{
public:
    FloorTable() {}
    FloorTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(FloorTable);
};

class SignTable : public UnaryMathTable
{
public:
    SignTable() {}
    SignTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SignTable);
};

class CosineHTable : public UnaryMathTable
{
public:
    CosineHTable() {}
    CosineHTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(CosineHTable);
};

class SineHTable : public UnaryMathTable
{
public:
    SineHTable() {}
    SineHTable(quint64 metaid, const Ilwis::OperationExpression &expr);
    static Ilwis::OperationImplementation *create(quint64 metaid,const Ilwis::OperationExpression& expr);
    static quint64 createMetadata();

    NEW_OPERATION(SineHTable);
};
}
}

#endif // UnaryMathTableOPERATIONS_H
