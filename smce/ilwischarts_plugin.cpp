#include "smce_plugin.h"
#include "kernel.h"
//#include "geodrawer.h"

#include <qqml.h>



void Chartsplugin::registerTypes(const char *uri)
{


    Ilwis::kernel()->issues()->log("Loaded module SMCE",Ilwis::IssueObject::itMessage);
}


