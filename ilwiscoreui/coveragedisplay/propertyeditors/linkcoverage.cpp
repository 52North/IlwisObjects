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

LinkCoverage::LinkCoverage()
{

}

LinkCoverage::LinkCoverage(VisualAttribute *p) : VisualPropertyEditor(p, "linkcoverage", TR("Link to other"), QUrl("LinkCoverage.qml"))
{
}

bool LinkCoverage::canUse(const IIlwisObject &obj, const QString &name) const
{
    if (name == VisualAttribute::GLOBAL_ONLY)
        return true;

    if (!obj.isValid())
        return false;
    if (!hasType(obj->ilwisType(), itCOVERAGE))
        return false;

    return !vpmodel()->layer()->isSupportLayer();
}

LayerModel * LinkCoverage::layer()
{
    return vpmodel()->layer();
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
