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

RepresentationElementModel::RepresentationElementModel(const IRepresentation& rpr, Raw raw, const QString &label, VisualPropertyEditor *parent) : QObject(parent), Identity(label), _rpr(rpr), _raw(raw)
{
}

QColor RepresentationElementModel::color() const
{
    if (_rpr.isValid()) {
        QColor clr =  _rpr->colors()->value2color(_raw);
        clr.setAlphaF(1);
        return clr;
    }
    return QColor();
}

void RepresentationElementModel::color(const QColor &clr)
{
    if (_rpr.isValid())
        _rpr->colors()->setColor(_raw, clr);
    emit ecolorChanged();
}

double RepresentationElementModel::opacity() const {
    QColor clr;
    if (_rpr.isValid())
        clr = _rpr->colors()->value2color(_raw);
    return clr.alphaF();
}
void RepresentationElementModel::opacity(double v) {
    if (v >= 0 && v <= 1) {
        QColor clr = color();
        clr.setAlphaF(v);
        color(clr);
        emit eopacityChanged();
    }
}


