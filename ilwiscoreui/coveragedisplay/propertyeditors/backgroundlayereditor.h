#ifndef BACKGROUNDLAYEREDITOR_H
#define BACKGROUNDLAYEREDITOR_H


#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{


class BackgroundLayerEditor : public VisualPropertyEditor
{
    Q_OBJECT

public:
    BackgroundLayerEditor();
    BackgroundLayerEditor(VisualAttribute *p);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualPropertyEditor *create(VisualAttribute *p);

};
}
}

#endif // BACKGROUNDLAYEREDITOR_H
