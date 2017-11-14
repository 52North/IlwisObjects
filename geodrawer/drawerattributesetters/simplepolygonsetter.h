#ifndef SIMPLEPOLYGONSETTER_H
#define SIMPLEPOLYGONSETTER_H

#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Ui{


class SimplePolygonSetter : public BaseSpatialAttributeSetter
{
public:
    SimplePolygonSetter(const LayerManager *manager);

	void getVertices(const SPFeatureI& feature, std::vector<qreal>& vertices, std::vector<int>& indices) const;

    void getColors(const VisualAttribute& attr,
                            const QVariant& value,const QColor& defaultColor,int start,
							std::vector<qreal>& colors) const;

};
}
}

#endif // SIMPLEPOLYGONSETTER_H
