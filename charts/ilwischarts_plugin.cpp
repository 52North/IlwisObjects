#include "ilwischarts_plugin.h"

#include <qqml.h>

#include "kernel.h" 
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "setseriescolor.h"
#include "chartoperationfactory.h"

using namespace Ilwis;
using namespace Ui;

void ChartsPlugin::registerTypes(const char *uri)
{
/*    ChartOperationFactory *factory = new ChartOperationFactory();
    factory->registerChartOperation("setseriescolor", SetSeriesColor::create);
//    factory->registerTableOperation("convertcolumndomain", ConvertColumnDomain::create);
//    factory->registerTableOperation("columnvisibility", ColumnVisibility::create);
//    factory->registerTableOperation("linktable", LinkTable::create);
    Ilwis::kernel()->addFactory(factory);
*/
    Ilwis::kernel()->issues()->log("Loaded Ilwis Charts module",Ilwis::IssueObject::itMessage);
}


