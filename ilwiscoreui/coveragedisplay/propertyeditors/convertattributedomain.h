#ifndef CONVERTATTRIBUTEDOMAIN_H
#define CONVERTATTRIBUTEDOMAIN_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis {
namespace Ui{
class ILWISCOREUISHARED_EXPORT ConvertAttributeDomain : public VisualPropertyEditor
{
        Q_OBJECT

    Q_PROPERTY(QStringList colorSchemes READ colorSchemes CONSTANT)

public:
    ConvertAttributeDomain();
    ConvertAttributeDomain(VisualAttribute *attr);
    bool canUse(const IIlwisObject &obj, const DataDefinition &) const;
    static VisualPropertyEditor *create(VisualAttribute *);

    Q_INVOKABLE bool execute(const QString& colorScheme, const QString& viewerId);
    Q_INVOKABLE QStringList allowedDomains() const;
    Q_INVOKABLE int domainTypeIndex();

    void prepare(const IIlwisObject &obj, const DataDefinition &datadef);
private:
    QStringList colorSchemes() const;
    DataDefinition _attributeDef;

 NEW_PROPERTYEDITOR(ConvertAttributeDomain)
};
}
}

#endif // CONVERTATTRIBUTEDOMAIN_H
