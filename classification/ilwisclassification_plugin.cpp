#include "ilwisclassification_plugin.h"

#include <qqml.h>

#include "kernel.h" 
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "supervisedclassificationmodel.h"


using namespace Ilwis;
using namespace Ui;

void ClassificationPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Ilwis::SClassification::SupervisedClassificationmodel>(uri, 1, 0, "SupervisedClassificationModel");
    Ilwis::kernel()->issues()->log("Loaded Ilwis Classification module",Ilwis::IssueObject::itMessage);
}


