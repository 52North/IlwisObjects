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

#ifndef NUMERICOPERATION_H
#define NUMERICOPERATION_H

namespace Ilwis {
class KERNELSHARED_EXPORT NumericOperation : public OperationImplementation
{
public:
    enum OperatorType{ otPLUS, otMINUS, otMULT, otDIV, otPOW, otMIN, otMAX, otUNKNOWN};

    NumericOperation();



protected:
    NumericOperation(quint64 metaid,const Ilwis::OperationExpression &expr);

    NumericRange *constructRangeFrom(const SPNumericRange& range1, const SPNumericRange& range2) const;
    NumericRange *constructRangeFrom(const SPNumericRange& range, double number) const;
    bool mathoperator(const QString& oper);
    double calc(double v1, double v2) const{
        if ( v1 != rUNDEF && v2 != rUNDEF) {
            switch(_operator) {
            case otPLUS:
                return v1 + v2;
                break;
            case otMINUS:
                return v1 - v2;
                break;
            case otDIV:
                if ( v2 != 0)
                    return v1 / v2;
                else
                    return rUNDEF;
                break;
            case otMULT:
                return v1 * v2;
                break;
            case otPOW:
                return std::pow(v1,v2);
                break;
            case otMIN:
                return std::min(v1,v2);
                break;
            case otMAX:
                return std::max(v1,v2);
                break;
            default:
                return rUNDEF;
            }
        }
        return rUNDEF;
    }

    double _number1 = rUNDEF;
    double _number2 = rUNDEF;
    bool _firstorder = true;
    OperatorType _operator = otUNKNOWN;
};
}

#endif // NUMERICOPERATION_H
