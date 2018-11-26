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

#ifndef PROJECTIONPARAMETERMODEL_H
#define PROJECTIONPARAMETERMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "projection.h"
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT ProjectionParameterModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString value READ value CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString valueType READ valueType CONSTANT)
    Q_PROPERTY(int parameterType READ parameterType CONSTANT)
public:
    ProjectionParameterModel();
    explicit ProjectionParameterModel(const Ilwis::IProjection&, Ilwis::Projection::ProjectionParamValue type, QObject *parent = 0);
    ~ProjectionParameterModel();

    QString name() const;
    QString value() const;
    QString description() const;
    QString valueType() const;
    int parameterType() const;


signals:

public slots:

private:
    QString _name;
    QString _value;
    QString _description;
    IlwisTypes _valueType;
    int _parmType = Ilwis::Projection::pvNONE;
};

#endif // PROJECTIONPARAMETERMODEL_H
