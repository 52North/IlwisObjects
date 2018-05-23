#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
#include "representationelementmodel.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "coveragelayermodel.h"
#include "visualpropertyeditor.h"

using namespace Ilwis;
using namespace Ui;

quint64 VisualPropertyEditor::_baseId = 0;

VisualPropertyEditor::VisualPropertyEditor(QObject *parent) : QObject(parent)
{

}

VisualPropertyEditor::VisualPropertyEditor(VisualAttribute *attr, const QString& name, const QString& displayNm, const QUrl &url) :
    QObject(attr), Ilwis::Identity(name, _baseId++),
    _qmlUrl(url),
    _displayName(displayNm),
    _visualAttribute(attr)
{
}

VisualPropertyEditor::VisualPropertyEditor(const VisualPropertyEditor& metadata, QObject *parent) : QObject(parent), Ilwis::Identity(metadata.name())
{
    _qmlUrl = metadata._qmlUrl;
}

Ilwis::Ui::VisualPropertyEditor::~VisualPropertyEditor()
{
}

QString VisualPropertyEditor::qmlUrl() const
{
    return _qmlUrl.toString();
}

QString VisualPropertyEditor::editorName() const
{
    return name();
}

QString VisualPropertyEditor::attributeName() const
{
    if ( vpmodel()){
        return vpmodel()->attributename();
    }
    return sUNDEF;
}

int VisualPropertyEditor::defaultHeight() const
{
    return 0;
}

bool VisualPropertyEditor::canUse(const IIlwisObject &, const DataDefinition &) const
{
    return false;
}

bool VisualPropertyEditor::canUse(const Ilwis::IIlwisObject &, const QString &) const
{
    return false;
}

void VisualPropertyEditor::prepare(const IIlwisObject &, const DataDefinition &)
{
}

int VisualPropertyEditor::layerIndex() const
{
    if (vpmodel() && vpmodel()->layer()) {
        return vpmodel()->layer()->nodeId();

    }
    return iUNDEF;
}

ICoverage VisualPropertyEditor::coverage() const
{
    if (vpmodel() && vpmodel()->layer()) {
        if (vpmodel()->layer()->layerType() == itFEATURELAYER || vpmodel()->layer()->layerType() == itRASTERLAYER) {
            const CoverageLayerModel *coverageLayer = static_cast<const CoverageLayerModel *>(vpmodel()->layer());
            return coverageLayer->coverage();
        }
    }
    return ICoverage();
}

QString VisualPropertyEditor::displayName() const
{
    return _displayName;
}

VisualAttribute *VisualPropertyEditor::vpmodel() const
{
    return _visualAttribute;
}

VisualAttribute *VisualPropertyEditor::vpmodel()
{
    return _visualAttribute;
}

void VisualPropertyEditor::attributesChanged(Raw , const QVariantMap& ){
    if ( _visualAttribute->layer()){
      //  _visualAttribute->layer()->drawer()->unprepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER);
     //   _visualAttribute->layer()->drawer()->prepare(Ilwis::Geodrawer::DrawerInterface::ptRENDER, IOOptions());
        _visualAttribute->layer()->redraw();
    }
}

QString VisualPropertyEditor::representationName() const
{
    if ( vpmodel()->representation().isValid())
        return vpmodel()->representation()->name();
    return "";
}

void VisualPropertyEditor::representationChanged(const IRepresentation &rpr)
{
    // empty implementation
}

QString Ilwis::Ui::VisualPropertyEditor::associatedUrl() const
{
    return _associatedUrl;
}

void Ilwis::Ui::VisualPropertyEditor::associatedUrl(const QString & url)
{
    _associatedUrl = url;
}

void VisualPropertyEditor::displayName(const QString& newname){
    _displayName = newname;
}




