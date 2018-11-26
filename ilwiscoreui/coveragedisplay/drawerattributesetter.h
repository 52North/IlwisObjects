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

#ifndef FEATURETYPEDRAWER_H
#define FEATURETYPEDRAWER_H
#include "ilwiscoreui_global.h"

#include <QVector>


namespace Ilwis {

class Feature;
class FeatureCoverage;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;
class SPFeatureI;
typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;

namespace Ui {

struct FeatureDrawing;

#define NEW_DRAWER_ATTRIBUTE_SETTER \
    private: \
        static DrawerAttributeSetter *dummy_attribute_setter;

#define REGISTER_DRAWER_ATTRIBUTE_SETTER(classname) \
    DrawerAttributeSetter *classname::dummy_attribute_setter=DrawerAttributeSetterFactory::registerDrawerAttributeSetter(#classname, classname::create);

class VisualAttribute;

class ILWISCOREUISHARED_EXPORT DrawerAttributeSetter
{
public:
    DrawerAttributeSetter(const IOOptions &options=IOOptions());
    ~DrawerAttributeSetter();

    virtual void getVertices(const geos::geom::Geometry *geometry, std::vector<qreal>& vertices, std::vector<int>& indices) const = 0;
    virtual void getColors(const VisualAttribute&,
                            const QVariant &value,const QColor& defaultColor,int start,
						     std::vector<qreal>& colors) const = 0;

};
}
}

#endif // FEATURETYPEDRAWER_H
