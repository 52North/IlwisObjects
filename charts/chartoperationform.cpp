#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/chartmodel.h"
#include "chartoperationform.h"

using namespace Ilwis;
using namespace Ui;

ChartOperationForm::ChartOperationForm(const QString& name, const QUrl &url, const QString& description, QObject *parent) : QObject(parent), Identity(name, i64UNDEF, sUNDEF, description), _qmlUrl(url)
{

}

void ChartOperationForm::prepare(ChartModel* model)
{
	_chartModel = model;
}

QString ChartOperationForm::qmlUrl() const
{
	return _qmlUrl.toString();
}

//ITable ChartOperationForm::table() const
//{
//	return chartModel()->table();
//}

ChartModel *ChartOperationForm::chartModel() const
{
	return _chartModel;
}




