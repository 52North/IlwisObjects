#ifndef POLYGONPROPERTYSETTER_H
#define POLYGONPROPERTYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class PolygonPropertySetter : public VisualPropertyEditor
{
    Q_OBJECT

    Q_PROPERTY(bool showAreas READ showAreas WRITE setShowAreas NOTIFY showAreasChanged)

public:
    PolygonPropertySetter();
    PolygonPropertySetter(VisualAttribute *p);
    static VisualPropertyEditor *create(VisualAttribute *p);
    bool canUse(const IIlwisObject &obj, const QString &name) const;
    bool showAreas() const;
    void setShowAreas(bool yesno);



signals:
    void showAreasChanged();

private:
};
}
}

#endif // POLYGONPROPERTYSETTER_H
