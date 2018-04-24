#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "modelregistry.h"
#include "tablemodel.h"
#include "chartmodel.h"
#include "mathhelper.h"

using namespace Ilwis;
using namespace Ui;

ChartModel::ChartModel() : QObject()
{
}

ChartModel::ChartModel(QObject *parent) : QObject(parent)
{
    _modelId = modelregistry()->newModelId();
    modelregistry()->registerModel(modelId(), "table", this);
}

quint32 Ilwis::Ui::ChartModel::createChart(const QString& name, const ITable & tbl, const QString & cType, quint32 xaxis, quint32 yaxis, quint32 zaxis)
{
    _table = tbl;
    _name = name;
    chartType(cType);
    return modelId();
}

QVariantList ChartModel::linkProperties() const
{
    return QVariantList();
}

quint32 ChartModel::modelId() const
{
    return _modelId;
}

QString ChartModel::name() const
{
    return _name;
}

QString Ilwis::Ui::ChartModel::mainPanelUrl()
{
    return "../../extensions/ui/Charts/qml/ChartsPanel.qml";
}

void Ilwis::Ui::ChartModel::chartType(const QString & tp)
{
    _chartType = tp;
    emit chartTypeChanged();
}

QString ChartModel::chartType() const
{
    return _chartType;
}




