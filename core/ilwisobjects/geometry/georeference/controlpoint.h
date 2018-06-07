#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

namespace Ilwis {
class KERNELSHARED_EXPORT ControlPoint : public Coordinate{
public:
    ControlPoint(bool invalid=false);
    ControlPoint(const Coordinate& crd, const Pixel& pix);
    ControlPoint(const Coordinate& crd, const Pixeld& pix );
    ControlPoint(const Coordinate& crd, const LatLon& ll);

    LatLon llLocation() const;
    void llLocation(const LatLon& ll);
    Pixeld gridLocation() const;
    Pixeld& gridLocationRef();
    void gridLocation(const Pixeld& pix);
    bool isActive() const;
    void active(bool yesno);
    bool isValid() const;
    double errorRow() const;
    double errorColumn() const;
    void errorRow(double err);
    void errorColumn(double rr);
private:
    Pixeld _gridLocation;
    LatLon  _llLocation;
    bool _active;
    bool _neverValid;
    double _errorRow = rUNDEF;
    double _errorColumn = rUNDEF;

};
}

#endif // CONTROLPOINT_H
