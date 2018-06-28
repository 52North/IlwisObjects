#include "ilwischarts_plugin.h"

#include <qqml.h>

#include "kernel.h" 
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "setseriescolor.h"
#include "chartmodel.h"
#include "chartoperationfactory.h"

using namespace Ilwis;
using namespace Ui;

void ChartsPlugin::registerTypes(const char *uri)
{
    Ilwis::kernel()->issues()->log("Loaded Ilwis Charts module",Ilwis::IssueObject::itMessage);
}


