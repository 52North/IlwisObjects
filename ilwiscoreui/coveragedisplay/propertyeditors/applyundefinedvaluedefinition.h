#ifndef APPLYUNDEFINEDVALUEDEFINITION_H
#define APPLYUNDEFINEDVALUEDEFINITION_H

#include <QObject>
#include "propertyeditors/visualpropertyeditor.h"

namespace Ilwis{
namespace Ui{


class ApplyUndefinedValueDefinition : public VisualPropertyEditor
{
      Q_OBJECT

    Q_PROPERTY(double undefinedvalue READ undefinedvalue WRITE undefinedvalue NOTIFY undefinedChanged)

public:
    ApplyUndefinedValueDefinition();
    ApplyUndefinedValueDefinition(VisualAttribute *p);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualPropertyEditor *create(VisualAttribute *);

    double undefinedvalue() const;
    void undefinedvalue(double tr);


     NEW_PROPERTYEDITOR(ApplyUndefinedValueDefinition)

signals:
         void undefinedChanged();
};
}
}

#endif // APPLYUNDEFINEDVALUEDEFINITION_H
