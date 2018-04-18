#include "coverage.h"
#include "table.h"
#include "attributemodel.h"
#include "visualattribute.h"
#include "visualpropertyeditor.h"
#include "coveragelayermodel.h"
#include "modelregistry.h"
#include "linkcoverage.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_PROPERTYEDITOR("linkcoverage", LinkCoverage)

LinkCoverage::LinkCoverage()
{

}

LinkCoverage::LinkCoverage(VisualAttribute *p) : VisualPropertyEditor(p, "linkcoverage", TR("Link to other"), QUrl("LinkCoverage.qml"))
{
}

bool LinkCoverage::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (!obj.isValid())
        return false;
    if (!hasType(obj->ilwisType(), itCOVERAGE))
        return false;

    return !vpmodel()->layer()->isSupportLayer();
}

CoverageLayerModel * LinkCoverage::coverage()
{
    CoverageLayerModel * lm = dynamic_cast<CoverageLayerModel *>(vpmodel()->layer());
    if (lm)
        return lm;
    return 0;

}

void LinkCoverage::setLink(const QVariantMap & parameters)
{
    quint32 sourceId = parameters["sourceid"].toUInt();
    quint32 targetId = parameters["targetid"].toUInt();
    std::pair<QString, QObject *> modelPair = modelregistry()->getModel(sourceId);
    if (modelPair.first != sUNDEF) {
        CoverageLayerModel *source = dynamic_cast<CoverageLayerModel *>(modelPair.second);
        if (!source)
            return;
        modelPair = modelPair = modelregistry()->getModel(targetId);
        if (modelPair.first == "featurecoverage") {
            CoverageLayerModel *targetMap = dynamic_cast<CoverageLayerModel *>(modelPair.second);
            if (!targetMap)
                return;
            QString linktype = parameters["linktype"].toString();
            if (source->supportsLinkType(linktype) && targetMap->supportsLinkType(linktype)) {
                source->connect(source, &CoverageLayerModel::linkSendMessage, targetMap, &CoverageLayerModel::linkAcceptMessage);
            }
        } else if (modelPair.first == "table" || modelPair.first == "flattable" || modelPair.first == "attributetable") {
            TableModel *targetTable = dynamic_cast<TableModel *>(modelPair.second);
            if (!targetTable)
                return;
            QString linktype = parameters["linktype"].toString();
            if (source->supportsLinkType(linktype) && targetTable->supportsLinkType(linktype)) {
                source->connect(source, &CoverageLayerModel::linkSendMessage, targetTable, &TableModel::linkAcceptMessage);
            }
        }
    }
}

VisualPropertyEditor *LinkCoverage::create(VisualAttribute *p)
{
    return new LinkCoverage(p);
}
