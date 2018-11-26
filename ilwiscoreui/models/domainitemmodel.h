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

#ifndef DomainItemModel_H
#define DomainItemModel_H

#include <QObject>

#include "kernel.h"
#include "ilwisdata.h"
#include "range.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "identifieritem.h"
#include "thematicitem.h"
#include "identifierrange.h"
#include "interval.h"
#include "intervalrange.h"
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT DomainItemModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString code READ code CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString minimum READ min CONSTANT)
    Q_PROPERTY(QString maximum READ max CONSTANT)

public:
    DomainItemModel();
    explicit DomainItemModel(Ilwis::SPItemRange _range, const QString &name, QObject *parent = 0);

    QString name() const;
    QString code() const;
    QString description() const;
    QString min() const;
    QString max() const;

signals:

public slots:

private:
    Ilwis::SPItemRange _range;
    QString _itemname;

};

#endif // DomainItemModel_H
