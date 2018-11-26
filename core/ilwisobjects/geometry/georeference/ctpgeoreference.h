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

#ifndef CTPGEOREFERENCE_H
#define CTPGEOREFERENCE_H

namespace Ilwis {



class KERNELSHARED_EXPORT CTPGeoReference : public GeoRefImplementation
{
public:
    CTPGeoReference(const QString &type);
    bool isValid() const;
    ControlPoint& controlPoint(quint32 index);
    quint32 nrControlPoints() const;
    qint32 index(const Pixel& pix) const;
    qint32 index(const Coordinate& crd) const;
    qint32 setControlPoint(const ControlPoint &pnt);
    void removeControlPoint(int index);
    int compute();
    virtual int minimumPointsNeeded() const = 0;
    void subPixelPrecision(bool yesno);
    bool subPixelPrecision() const;
    static QString typeName();

protected slots:
    void copyTo(GeoRefImplementation *impl);
private:
    std::vector<ControlPoint> _controlPoints;
    ControlPoint _invalidCP;
    bool _subPixelPrecision = false;
};
}

#endif // CTPGEOREFERENCE_H
