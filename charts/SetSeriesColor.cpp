#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "models/tablemodel.h"
#include "chartoperationform.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operationExpression.h"
#include "operation.h"
#include "setseriescolor.h"

using namespace Ilwis;
using namespace Ui;

SetSeriesColor::SetSeriesColor() : ChartOperationForm("Set dataseries color", QUrl("SetSeriesColor.qml"), TR("Changes the color of the selected dataseries in the chart"))
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
		QString colName = parameters["columnname"].toString();
		QString targetdom = parameters["targetdomaintype"].toString();
        QString expr;// = QString("chartsetcolor(%1,%2,%3)").arg(table()->resource().url().toString()).arg(colName).arg(targetdom);

		Ilwis::OperationExpression ex(expr);
		Ilwis::Operation op(ex);
		ExecutionContext ctx;
		SymbolTable tbl;
		if (op->execute(&ctx, tbl)) {
		//	chartModel()->updateSeries();
		}
	}
	catch (const ErrorObject&) {

	}
}

/*TableOperation *ConvertColumnDomain::create()
{
return new ConvertColumnDomain();
}*/
