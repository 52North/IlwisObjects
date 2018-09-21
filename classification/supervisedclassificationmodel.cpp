#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "modeller/applicationmodel.h"
#include "modeller/model.h"
#include "workflow/modelbuilder.h"
#include "supervisedclassificationmodel.h"


using namespace Ilwis::Ui;
using namespace Ilwis;
using namespace SClassification;

REGISTER_ANALYSISMODEL(Ilwis::SClassification::SupervisedClassificationmodel, "supervisedclassification")

SupervisedClassificationmodel::SupervisedClassificationmodel()
{

}

SupervisedClassificationmodel::SupervisedClassificationmodel(Ilwis::AnalysisPattern *p) : AnalysisModel(p){
    _panels["main"] = "Classification/qml/SupervisedClassificationPanel.qml";
    _panels["form"] = "Classification/qml/SupervisedClassificationForm.qml";
}

AnalysisModel *SupervisedClassificationmodel::create(Ilwis::AnalysisPattern *pattern)
{
    return new SupervisedClassificationmodel(pattern);
}

QVariantMap SupervisedClassificationmodel::execute(const QVariantMap parameters)
{
    QVariantMap output;
    if ( _analysis){
        _analysis->execute(parameters, output);
    }
    return output;
}

