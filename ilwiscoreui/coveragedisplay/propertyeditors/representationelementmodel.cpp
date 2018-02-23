#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "representationelementmodel.h"
#include "visualpropertyeditor.h"

using namespace Ilwis;
using namespace Ui;

RepresentationElementModel::RepresentationElementModel()
{

}

RepresentationElementModel::RepresentationElementModel(VisualPropertyEditor *p) : QObject(p)
{
}

RepresentationElementModel::RepresentationElementModel(const QString &label, VisualPropertyEditor *p) : QObject(p), Identity(label)
{
}

RepresentationElementModel::RepresentationElementModel(Raw raw, const QString &label, VisualPropertyEditor *parent) : QObject(parent), Identity(label), _raw(raw)
{
}

QColor RepresentationElementModel::color() const
{
    return _color;
}

void RepresentationElementModel::color(const QColor &clr)
{
    _color = clr;
    emit ecolorChanged();
}

double RepresentationElementModel::opacity() const {
    return 1.0 - _color.alphaF();
}
void RepresentationElementModel::opacity(double v) {
    if (v >= 0 && v <= 1) {
        _color.setAlphaF(1.0 - v);
        emit eopacityChanged();
    }
}


