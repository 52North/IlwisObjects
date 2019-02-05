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
#include "geos/geom/Coordinate.h"
#include "numericrange.h"
#include "ilwiscoordinate.h"
#include "eigen3/Eigen/Dense"
#include "mathhelper.h"

using namespace Ilwis;

#define MAXTERMS 10

MathHelper::MathHelper()
{
}

int MathHelper::findOblique(int iPoints,
              const std::vector<Coordinate> &independent, const std::vector<Coordinate> &dependent, std::vector<Coefficient> &coef, bool useCols)

{
    if (iPoints < 4) return -2;
    int N = 2 * iPoints;
    int M = 8;

    Eigen::MatrixXd A(N, M);
    Eigen::VectorXd b(N);

    for (int i = 0; i < iPoints; ++i) {
        for (int j = 0; j < 8; ++j) {
            A(2*i,j) = 0;
            A(2*i+1,j) = 0;
        }
        A(2*i, 0) = independent[i].x;
        A(2*i, 1) = independent[i].y;
        A(2*i, 2) = 1;
        A(2*i, 6) = - dependent[i].x * independent[i].x;
        A(2*i, 7) = - dependent[i].x * independent[i].y;
        A(2*i+1  , 3) = independent[i].x;
        A(2*i+1  , 4) = independent[i].y;
        A(2*i+1  , 5) = 1;
        A(2*i+1  , 6) = - dependent[i].y * independent[i].x;
        A(2*i+1  , 7) = - dependent[i].y * independent[i].y;
        b(2*i) = dependent[i].x;
        b(2*i+1  ) = dependent[i].y;
    }
    Eigen::VectorXd sol = A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);
    for (int i = 0; i < 8; ++i) {
        double vb = sol(i);
        if ( useCols)
            coef[i]._x = vb;
        else
            coef[i]._y = vb;
    }
    return 0;
}

int MathHelper::findPolynom(int iTerms, int iPoints, const std::vector<Coordinate> &independent, const std::vector<Coordinate> &dependent, std::vector<Coefficient> &coef)
{
    double Matrix[MAXTERMS][MAXTERMS], InvMat[MAXTERMS][MAXTERMS];
    double DU[MAXTERMS], DV[MAXTERMS], PolyProd[MAXTERMS];
    double D0, D1;
    int point, row, col, term, MaxPower, i;

    const int PowerX[] = {0,1,0,1,2,0,3,2,1,0};
    const int PowerY[] = {0,0,1,1,0,2,0,1,2,3};
    const double MinVal = 1E-10; // was originally (in Fortran) 1e-10 and since ILWIS.1 (Pascal) it was 1e-3

    //  Set no. of terms in Polynomial and Valid Order

    if ( iPoints < iTerms ) return -2;

    MaxPower = PowerY[iTerms-1];

    std::vector<double> DX(4);
    std::vector<double> DY(4);

    //  Initialize Matrix and Dependent Vectors to Zero
    //  and Create Identity Matrix for Inversion
    for (term = 0; term < iTerms; ++term)
    {
        DU[term] = 0;
        DV[term] = 0;
        for (row = 0; row < iTerms; ++row)
        {
            Matrix[term][row] = 0;
            InvMat[term][row] = 0;
        }
        InvMat[term][term] = 1;
    }

    //  Build Matrix and Dependent Vector
    for ( point = 0; point < iPoints; ++point)
    {

        //	Set up Products of X and Y
        DX[0] = 1;
        DY[0] = 1;
        for (i = 0; i < MaxPower; ++i)
        {
            DX[i+1] = DX[i] * independent[point].x;
            DY[i+1] = DY[i] * independent[point].y;
        }
        for (term = 0; term < iTerms; ++term)
            PolyProd[term] = DX[PowerX[term]] * DY[PowerY[term]];

        //	Increment First Diagonal term in Matrix
        //	and First Element in Both Dependent Vectors
        Matrix[0][0] += 1;
        DU[0] += dependent[point].x;
        DV[0] += dependent[point].y;

        //	Increment Next Diagonal term in Matrix
        //	and Next Elements in Dependent Vectors
        for (row = 1; row < iTerms; ++row)
        {
            D0 = PolyProd[row];
            Matrix[row][row] += D0 * D0;
            DU[row] += dependent[point].x * D0;
            DV[row] += dependent[point].y * D0;

            //  Increment Remainder of row up to Diagonal term
            //  and Copy to Corresponding column
            for (col = 0; col < row; ++col)
            {
                Matrix[row][col] += D0 * PolyProd[col];
                Matrix[col][row] = Matrix[row][col];
            } // col
        } // row
    } // point

    //  Start Matrix Inversion, points Next Diagonal Element Too Small ?
    for (term = 0; term < iTerms; ++term)
    {
        D0 = Matrix[term][term];
        if (abs(D0) <= MinVal) return -3;

        //	Divide This row by its Diagonal Element
        for (row = 0; row < iTerms; ++row)
        {
            Matrix[term][row] /= D0;
            InvMat[term][row] /= D0;
        }

        //	Subtract Appropriate Multiple of This row from All Other rows
        for (row = 0; row < iTerms; ++row)
            if (row != term )
            {
                D1 = Matrix[row][term];
                for ( col = 0; col < iTerms; ++col)
                {
                    Matrix[row][col] -= Matrix[term][col] * D1;
                    InvMat[row][col] -= InvMat[term][col] * D1;
                } // col
            } // row
    } // term

    for (term = 0; term < MAXTERMS; ++term) {
        coef[term]._x = 0;
        coef[term]._y = 0;
    }
    //  Apply Inverse to Both Dependent Vectors and Give Coefficients
    for (term = 0; term < iTerms; ++term)
    {
        D0 = 0;
        D1 = 0;
        for (row = 0; row < iTerms; ++row)
        {
            D0 += DU[row] * InvMat[term][row];
            D1 += DV[row] * InvMat[term][row];
        }
        coef[term]._x = D0;
        coef[term]._y = D1;
    }
    return 0;
}

NumericRange MathHelper::roundRange(double rmin, double rmax)
{
    double tickLimits []  = {0,0.1,0.2,0.25,0.5,1.0};
    double range = rmax - rmin;
    //long d =  abs(log10(range)) + 1;
    long d =  log10(range) + 1;
    double step = range / pow(10.0,abs(d));
    int i = 0;
    while(i < 6) {
        if ( step < tickLimits[i]) {
            step = tickLimits[i];
            break;
        }
        ++i;
    }
    step = step * pow(10.0,d - 1);
    double lower = step * round(rmin / step);
    double intpart;
    double r = modf (rmax / step , &intpart);
    double upper = step * round(r ==0 ? intpart : 1 + intpart );
    return NumericRange(lower, upper, step);
}

double MathHelper::round(double r)
{
    if (r < 7)
      if (r < 1e-10)
        return 1e-10;
      else
        return round(r*10)/10;
    else if (r > 70)
      if (r > 1e30)
        return 1e30;
      else
        return round(r/10)*10;
    else if (r < 17)
      return 10;
    else if (r <= 25)
      return 20;
    else
      return 50;
}


double MathHelper::roundTo3DecimalDigits(const double value) {
    return ((qint64)(value * 1000.0)) / 1000.0 ;
}

quint8 MathHelper::lenDecimalDigits(double number) {
    const double eps = 1e-9; // we are going to miss anything smaller than this, but we also don't want to be fooled by cases like 0.00000000001
    quint8 digits = 0;
    while(eps < fabs(number - ::round(number))) { // explicitly call round() from math.h
        number *= 10.0;
        ++digits;
    }
    return digits;
}

LogicalOperator MathHelper::string2logicalOperator(const QString& op){
    QString optemp = op.trimmed().toLower();
    if (optemp == "and")
        return loAND;
    else if ( optemp == "or")
        return loOR;
    else if ( optemp == "equals")
        return loEQ;
    else if ( optemp == "notequals")
        return loNEQ;
    else if ( optemp == "not")
        return loNOT;
    else if ( optemp == "lessthan")
        return loLESS;
    else if ( optemp == "lessorequal")
        return loLESSEQ;
    else if ( optemp == "greaterthan")
        return loGREATER;
    else if (optemp == "greaterorequal")
        return loGREATEREQ;
    else if (optemp == "exclusiveor")
        return loXOR;
    return loNONE;
}

QString MathHelper::logicalOperator2string(LogicalOperator lo){
    switch (lo){
    case loAND:
        return "and";
    case loOR:
        return "or";
    case loNOT:
        return "not";
    case loEQ:
        return "equals";
    case loNEQ:
        return "notequals";
    case loLESS:
        return "lessthan";
    case loLESSEQ:
        return "lessorequal";
    case loGREATER:
        return "greaterthan";
    case loGREATEREQ:
        return "greaterorequal";
    case loXOR:
        return "exclusiveor";
    default:
        return "";

    }
    return sUNDEF;
}
