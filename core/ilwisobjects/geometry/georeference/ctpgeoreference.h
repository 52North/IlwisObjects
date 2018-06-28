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
