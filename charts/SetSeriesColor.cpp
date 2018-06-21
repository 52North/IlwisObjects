#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/tablemodel.h"
#include "chartoperationeditor.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operation.h"
#include "chartmodel.h"
#include "setseriescolor.h"

using namespace Ilwis;
using namespace Ui;

REGISTER_CHARTPROPERTYEDITOR("setseriescolor", SetSeriesColor)

SetSeriesColor::SetSeriesColor() : ChartOperationEditor("setseriescolor", QUrl("SetSeriesColor.qml"), TR("Changes the color of the selected dataseries in the chart"))
{
}

bool SetSeriesColor::canUse(ChartModel *model, const QVariantMap &parameter) const
{
	return true;
/*	bool ok;
	quint32 colIndex = parameter["columnindex"].toInt(&ok);
	if (!ok || colIndex >= chartModel()->getColumnCount())
		return false;
	IDomain dom = chartModel()->table()->columndefinition(colIndex).datadef().domain();

	return hasType(dom->ilwisType(), itTEXTDOMAIN);*/
}

void SetSeriesColor::execute(const QVariantMap &parameters)
{
	try {
		QString seriesName = parameters["seriesname"].toString();
		QString color = parameters["color"].toString();
        quint32 cid = chartModel()->modelId();
        QString expr = QString("chartseriescolor(%1,\"%2\",\"%3\")").arg(cid).arg(seriesName).arg(color);

		Ilwis::OperationExpression ex(expr);
		Ilwis::Operation op(ex);
		ExecutionContext ctx;
		SymbolTable tbl;
		if (op->execute(&ctx, tbl)) {
			chartModel()->updateSeriesChanged();
		}
	}
	catch (const ErrorObject&) {

	}
}
