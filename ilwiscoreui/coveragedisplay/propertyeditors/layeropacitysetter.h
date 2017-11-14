#ifndef LAYERTRANSPARENCYSETTER_H
#define LAYERTRANSPARENCYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class LayerOpacitySetter : public VisualPropertyEditor
{
    Q_OBJECT

    Q_PROPERTY(float opacity READ opacity WRITE opacity NOTIFY opacityChanged)
public:
    LayerOpacitySetter();
    LayerOpacitySetter(VisualAttribute *p);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualPropertyEditor *create(VisualAttribute *p);

    float opacity() const;
    void opacity(float tr);


  NEW_PROPERTYEDITOR(LayerOpacitySetter)
signals:
      void opacityChanged();

private:
};
}
}

#endif // LAYERTRANSPARENCYSETTER_H
