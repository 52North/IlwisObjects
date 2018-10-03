#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/chartmodel.h"
#include "chartoperationeditor.h"

using namespace Ilwis;
using namespace Ui;

ChartOperationEditor::ChartOperationEditor(const QString& name, const QString& displayName, const QUrl &url, const QString& description, QObject *parent) :
    QObject(parent), Identity(name, i64UNDEF, sUNDEF, description), _qmlUrl(url), _displayName(displayName)
{

}

void ChartOperationEditor::prepare(ChartModel* model)
{
	_chartModel = model;
}

QString ChartOperationEditor::qmlUrl() const
{
	return _qmlUrl.toString();
}

QString ChartOperationEditor::displayName() const {
    return _displayName;
}

ChartModel *ChartOperationEditor::chartModel() const
{
	return _chartModel;
}




