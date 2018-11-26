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

#ifndef CATALOGMAPITEM_H
#define CATALOGMAPITEM_H

#include "coverage.h"
#include "georeference.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
class Coverage;
typedef IlwisData<Coverage> ICoverage;

}
class ILWISCOREUISHARED_EXPORT CatalogMapItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
public:
    CatalogMapItem();
    CatalogMapItem(const std::vector<Ilwis::Resource>& res, const Ilwis::IGeoReference &grf, QObject* parent);
    ~CatalogMapItem();

    QString latLonEnvelope() const;
    QString catalogItemType();
    QString name() const;

    Q_INVOKABLE QVariantMap drawEnvelope() const;
private:
    Ilwis::BoundingBox _boundingBox;
    std::vector<Ilwis::Resource> _resources;
    Ilwis::IGeoReference _screenGrf;


};

#endif // CATALOGMAPITEM_H
