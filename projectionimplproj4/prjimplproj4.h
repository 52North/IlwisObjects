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

#ifndef PROJECTIONIMPLEMENTATIONPROJ4_H
#define PROJECTIONIMPLEMENTATIONPROJ4_H

namespace Ilwis {
class ProjectionImplementationProj4 : public ProjectionImplementation
{
public:
    ProjectionImplementationProj4(const Ilwis::Resource &resource);
     ProjectionImplementationProj4(const QString& projtype,const QString &code);
    ~ProjectionImplementationProj4();
    Coordinate latlon2coord(const LatLon&) const;
    LatLon coord2latlon(const Coordinate&) const;
    static bool canUse(const Ilwis::Resource &) { return true;}
    static ProjectionImplementation *create(const Ilwis::Resource &resource);
     bool compute() { return true; }
     void setParameter(Projection::ProjectionParamValue type, const QVariant& value);
     void setEllipsoid(const Ilwis::IEllipsoid &ell);
     bool prepare(const QString& parms="");
     QString toProj4() const;
     bool canConvertToLatLon() const;
     bool canConvertToCoordinate() const;
     virtual ProjectionImplementation* clone();
 protected:
     virtual void copyTo(ProjectionImplementation *prj);
private:
    ProjectionImplementationProj4();
    QString getParam(const QString & proj4def, const QString & parm) const;
    QString getEllips(const QString & proj4def) const;
    QString _targetDef;
    projPJ  _pjLatlon;
    projPJ  _pjBase;
    bool _outputIsLatLon;
    void fillParameters(const QString &code);
    void removeParameter(const QString &parm);
};
}

#endif // PROJECTIONIMPLEMENTATIONPROJ4_H
