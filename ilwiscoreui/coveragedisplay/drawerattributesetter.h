#ifndef FEATURETYPEDRAWER_H
#define FEATURETYPEDRAWER_H
#include "ilwiscoreui_global.h"

#include <QVector>

namespace Ilwis {

class Feature;
class FeatureCoverage;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;
class SPFeatureI;


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

    virtual void getVertices(const SPFeatureI& feature, std::vector<qreal>& vertices, std::vector<int>& indices) const = 0;
    virtual void getColors(const VisualAttribute&,
                            const QVariant &value,const QColor& defaultColor,int start,
						     std::vector<qreal>& colors) const = 0;

};
}
}

#endif // FEATURETYPEDRAWER_H
