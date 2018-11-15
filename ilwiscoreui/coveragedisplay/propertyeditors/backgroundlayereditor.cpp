#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
#include "geometries.h"
#include "backgroundlayereditor.h"


using namespace Ilwis;
using namespace Ui;

BackgroundLayerEditor::BackgroundLayerEditor()
{

}

BackgroundLayerEditor::BackgroundLayerEditor(VisualAttribute *p) :
    VisualPropertyEditor(p, "backgroundlayereditor",TR("Background Layer"),QUrl("BackgroundLayerEditor.qml"))
{

}

bool BackgroundLayerEditor::canUse(const IIlwisObject& obj, const QString& name ) const
{
    return name == "backgroundlayereditor";
}

VisualPropertyEditor *BackgroundLayerEditor::create(VisualAttribute *p)
{
    return new BackgroundLayerEditor(p);
}

