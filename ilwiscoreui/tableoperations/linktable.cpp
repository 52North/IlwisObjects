#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/tablemodel.h"
#include "tableoperation.h"
#include "modelregistry.h"
#include "coveragelayermodel.h"
#include "linktable.h"

using namespace Ilwis;
using namespace Ui;

LinkTable::LinkTable() :TableOperation("Link Table", QUrl("LinkTable.qml"), TR("Determines the parameters of a link between a table and another object")) {

}
bool LinkTable::canUse(TableModel *tblModel, const QVariantMap &parameter) const {
    return parameter.contains("tableonly");
}

void LinkTable::execute(const QVariantMap &parameters) {
    quint32 sourceId = parameters["sourceid"].toUInt();
    quint32 targetId = parameters["targetid"].toUInt();
    std::pair<QString, QObject *> modelPair = modelregistry()->getModel(sourceId);
    if (modelPair.first != sUNDEF) {
        TableModel *sourceTable = dynamic_cast<TableModel *>(modelPair.second);
        if (!sourceTable)
            return;
        modelPair = modelPair = modelregistry()->getModel(targetId);
        if (modelPair.first == "featurecoverage") {
            CoverageLayerModel *targetMap = dynamic_cast<CoverageLayerModel *>(modelPair.second);
            if (!targetMap)
                return;
            QString linktype = parameters["linktype"].toString();
            if (sourceTable->supportsLinkType(linktype) && targetMap->supportsLinkType(linktype)) {
                sourceTable->connect(sourceTable, &TableModel::linkSendMessage, targetMap, &CoverageLayerModel::linkAcceptMessage);
            }
        }
    }
}

Ilwis::Ui::TableOperation *LinkTable::create() {
    return new LinkTable();
}