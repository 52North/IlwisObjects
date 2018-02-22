#ifndef REPRESENTATIONELEMENT_H
#define REPRESENTATIONELEMENT_H

#include <QObject>
#include <QColor>
#include "identity.h"
#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Ui{
class VisualPropertyEditor;

class ILWISCOREUISHARED_EXPORT RepresentationElementModel : public QObject, public Ilwis::Identity{
    Q_OBJECT

public:
    RepresentationElementModel();
    explicit RepresentationElementModel(VisualPropertyEditor *p);
    RepresentationElementModel(const QString& label, VisualPropertyEditor *parent=0);
    RepresentationElementModel(Raw raw,const QString& label, VisualPropertyEditor *parent=0);
    Q_PROPERTY(QColor ecolor READ color WRITE color NOTIFY ecolorChanged)
    Q_PROPERTY(QString label READ name CONSTANT)
    Q_PROPERTY(double eopacity READ opacity NOTIFY eopacityChanged)

    QColor color() const;
    void color(const QColor& clr);
    double opacity() const;
    void opacity(double v);

signals:
    void ecolorChanged();
    void eopacityChanged();
    void attributeChanged(int index, const QVariantMap& values);

private:
    QColor _color;
    Raw _raw;
    double _opacity;
};
}
}

#endif // REPRESENTATIONELEMENT_H
