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

#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

namespace Ilwis {
class KERNELSHARED_EXPORT ControlPoint : public Coordinate{
public:
    ControlPoint(bool invalid=false);
    ControlPoint(const Coordinate& crd, const Pixel& pix);
    ControlPoint(const Coordinate& crd, const Pixeld& pix );
    ControlPoint(const Coordinate& crd, const LatLon& ll);
	ControlPoint(const Coordinate& crd, const LatLon& ll, const Pixeld& pix);

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
    bool _active = true;
    bool _neverValid = false;
    double _errorRow = rUNDEF;
    double _errorColumn = rUNDEF;

};
}

#endif // CONTROLPOINT_H
