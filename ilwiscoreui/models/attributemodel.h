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

#ifndef ATTRIBUTEMODEL_H
#define ATTRIBUTEMODEL_H

#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "representation.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Ui {


class ILWISCOREUISHARED_EXPORT AttributeModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString attributename READ attributename CONSTANT)
    Q_PROPERTY(QString defaultRangeDefinition READ defaultRangeDefinition CONSTANT)
    Q_PROPERTY(QString actualRangeDefintion READ actualRangeDefintion CONSTANT)
	Q_PROPERTY(QString range READ range CONSTANT)
    Q_PROPERTY(QString attributeDomain READ attributeDomain CONSTANT)
    Q_PROPERTY(QString attributeValueType READ attributeValueType CONSTANT)
    Q_PROPERTY(QString attributeDomainType READ attributeDomainType CONSTANT)
	Q_PROPERTY(QString attributeInternalDomainType READ attributeInternalDomainType CONSTANT)
	Q_PROPERTY(QString representationId READ representationId CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)

public:
    AttributeModel();
    virtual ~AttributeModel();
    explicit AttributeModel(const DataDefinition& def, const QString& name, QObject *parent);
    explicit AttributeModel(const DataDefinition& def, const QString& name, QObject *parent, const IIlwisObject& owner);

    QString defaultRangeDefinition() const;
    QString actualRangeDefintion(bool calc=false) const;
    virtual QString attributename() const;
    QString attributeDomain() const;
    QString attributeValueType() const;
    QString attributeDomainType() const;
	QString attributeInternalDomainType() const;
    Q_INVOKABLE QStringList attributeValues(bool defaultRange) const;
    Q_INVOKABLE QString rangeDefinition(bool defaultRange, bool calc, const QString&) const;

   const DataDefinition& datadefinition() const;

protected:
    void setObject(const Ilwis::IIlwisObject& obj);
    QString icon();

    Ilwis::DataDefinition _datadef;
    QString _name;
    QStringList _attributeValuesDefaultRange;
    QStringList _attributeValuesActualRange;
    IIlwisObject _owner;

	QString range() const;
	QString representationId() const;

signals:

public slots:



};
}
}

#endif // ATTRIBUTEMODEL_H
