#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "representation.h"
#include "representationelement.h"
#include "visualpropertyeditor.h"

using namespace Ilwis;
using namespace Ui;

RepresentationElement::RepresentationElement()
{

}

RepresentationElement::RepresentationElement(VisualPropertyEditor *p) : QObject(p)
{
}

RepresentationElement::RepresentationElement(const QString &label, VisualPropertyEditor *p) : QObject(p), Identity(label)
{
}

RepresentationElement::RepresentationElement(Raw raw, const QString &label, const QColor &clr, VisualPropertyEditor *parent) : QObject(parent), Identity(label), _color(clr), _raw(raw)
{
}

QColor RepresentationElement::color() const
{
    return _color;
}

void RepresentationElement::color(const QColor &clr)
{
    _color = clr;
    emit ecolorChanged();
}

