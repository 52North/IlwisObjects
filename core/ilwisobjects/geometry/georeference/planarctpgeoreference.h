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

#ifndef PLANARCTPGEOREFERENCE_H
#define PLANARCTPGEOREFERENCE_H

namespace Ilwis{



class KERNELSHARED_EXPORT PlanarCTPGeoReference : public CTPGeoReference
{
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    enum Transformation { tCONFORM, tAFFINE, tSECONDORDER, tFULLSECONDORDER,
                    tTHIRDORDER, tPROJECTIVE, tUNKNOWN } ;
    PlanarCTPGeoReference();
    PlanarCTPGeoReference(const Resource& resource);
    virtual Coordinate pixel2Coord(const Pixeld &pix) const;
    virtual Pixeld coord2Pixel(const Coordinate& crd) const;
    bool isValid() const;
    virtual bool isLinear() const;
    virtual double pixelSize() const;
    virtual int compute();
    void transformation(Transformation tr);
    Transformation transformation() const;
    GeoRefImplementation *clone();
    static QString typeName();
	QString grfType() const;
    int minimumPointsNeeded() const;
    double sigma() const;
    void sigma(double s);

    static GeoRefImplementation *create();
private:
    Eigen::Matrix2d _jacobian;
    double _sigma = rUNDEF;
    Transformation _transformation = tAFFINE;
    std::vector<Coefficient> _colrowCoef;
    std::vector<Coefficient> _xyCoef;
    Coefficient _avgCrd;
    Coefficient _avgPix;

    Coordinate crdInverseOfAffine(const Pixeld &pix) const;
    Coordinate crdInverseOfProjective(const Pixeld &pix) const;
    Coordinate crdInverseOfHigherOrder(const Pixeld &pix);
    void makeJacobianMatrix(const Coordinate &crdIn, Eigen::Matrix2d &rmJ);
    void copyTo(GeoRefImplementation *impl);


};
}

#endif // PLANARCTPGEOREFERENCE_H
