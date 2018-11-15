#ifndef OPENATTRIBUTETABLE_H
#define OPENATTRIBUTETABLE_H


#include <QObject>
#include <QQmlListProperty>
#include "visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class OpenAttributeTable : public VisualPropertyEditor
{
    Q_OBJECT

    Q_PROPERTY(QStringList attributeTable READ attributeTable CONSTANT)

public:
    OpenAttributeTable();
    OpenAttributeTable(VisualAttribute *p);

    bool canUse(const IIlwisObject &obj, const QString &name) const;

    QStringList attributeTable() const;

    static VisualPropertyEditor *create(VisualAttribute *p);



private:
};
}
}

#endif // OPENATTRIBUTETABLE_H
