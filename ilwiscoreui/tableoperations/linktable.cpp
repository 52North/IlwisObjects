/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

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