#ifndef SIMPLELINESETTER_H
#define SIMPLELINESETTER_H

#include "basespatialattributesetter.h"

namespace Ilwis {
namespace Ui{

class SimpleLineSetter : public BaseSpatialAttributeSetter
{
public:
    SimpleLineSetter(const LayerManager *manager);
    ~SimpleLineSetter();


	void getVertices(const SPFeatureI& feature, std::vector<qreal>& vertices, std::vector<int>& indices) const;

    void getColors(const VisualAttribute& attr,
                            const QVariant& value,const QColor& defaultColor,int start,
							std::vector<qreal>& colors) const;

};
}
}

#endif // SIMPLELINESETTER_H
