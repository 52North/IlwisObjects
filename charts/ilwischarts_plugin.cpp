#include "ilwischarts_plugin.h"
//#include "geodrawer.h"

#include <qqml.h>

#include "kernel.h" 
#include "ilwisdata.h"


void ChartsPlugin::registerTypes(const char *uri)
{
 
    Ilwis::kernel()->issues()->log("Loaded module Ilwis Charts",Ilwis::IssueObject::itMessage);
}


