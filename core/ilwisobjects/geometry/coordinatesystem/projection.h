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

#ifndef PROJECTION_H
#define PROJECTION_H

#include "kernel_global.h"

namespace Ilwis {

class ProjectionImplementation;
class ConventionalCoordinateSystem;

class KERNELSHARED_EXPORT Projection : public IlwisObject
{
public:
     enum ProjectionParamValue { pvNONE, pvFALSEEASTING, pvFALSENORTHING, pvCENTRALMERIDIAN,
     pvLATITUDEOFTRUESCALE, pvCENTRALPARALLEL, pvSCALE, pvNORTH,
     pvZONE, pvSTANDARDPARALLEL1, pvSTANDARDPARALLEL2, pvHEIGHT,
     pvTILTED, pvTILT, pvAZIMYAXIS, pvAZIMCLINE,
     pvPOLE, pvNORIENTED, pvELLCODE, pvLAST };

    enum FindMatch{fmNoItem, fmNoValue,fmMatch};

    Projection();
    Projection(const Resource &res);
    virtual ~Projection();

    virtual Coordinate latlon2coord(const LatLon&) const;
    virtual LatLon coord2latlon(const Coordinate&) const;

    bool prepare(const QString& parms);
    bool prepare(const IOOptions& options=IOOptions());
    void setImplementation(ProjectionImplementation *impl);

    QString authority() const;
    void setAuthority(const QString &auth);
    QString wkt() const;
    void setWkt(const QString &wkt);
    QString toProj4() const;

    IlwisTypes ilwisType() const;
    QString toWKT(quint32 spaces=0);

    QVariant parameter(Projection::ProjectionParamValue type) const;
    void setParameter(Projection::ProjectionParamValue type, const QVariant& value);


    bool isEqual(const IlwisObject* obj) const;
    bool isValid() const;
    bool canConvertToLatLon() const;
    bool canConvertToCoordinate() const;

    void setCoordinateSystem(ConventionalCoordinateSystem *csy);
    bool isSet(Projection::ProjectionParamValue type) const;
    bool isUsed(Projection::ProjectionParamValue type) const;
    IlwisTypes valueType(Projection::ProjectionParamValue type) const;

    static QString parameterName(Projection::ProjectionParamValue pv) ;
    static Projection::ProjectionParamValue parameterName2type(const QString &name);
    static QString projectionCode2Name(const QString& code);

    QStringList parameterNameList() const;
    IlwisObject *clone();
protected:
    void copyTo(IlwisObject *obj);
    static Projection::FindMatch find(const QString& v1, const QString& value) ;

private:
    QString _wkt;
    QString _authority;
    QScopedPointer<ProjectionImplementation> _implementation;
    static std::map<QString, QString> _projcode2Name;
    void fillProjectionCodes();
};
typedef Ilwis::IlwisData<Ilwis::Projection> IProjection;
}

Q_DECLARE_METATYPE(Ilwis::IProjection)



#endif // PROJECTION_H
