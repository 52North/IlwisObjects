#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
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
#include "supervisedclassification.h"

using namespace Ilwis;
using namespace SClassification;

REGISTER_ANALYSISPATTERN(SupervisedClassification)

SupervisedClassification::SupervisedClassification() : Ilwis::AnalysisPattern()
{
    Init();
}

void SupervisedClassification::Init()
{
}

bool SupervisedClassification::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
  return true;
}

IWorkflow SupervisedClassification::workflow(const IOOptions &opt)
{
    return IWorkflow();
}

void SupervisedClassification::store(QDataStream &stream)
{
    stream << type();
    AnalysisPattern::store(stream);
}

void SupervisedClassification::loadMetadata(QDataStream &stream)
{
    AnalysisPattern::loadMetadata(stream);
}

QString SupervisedClassification::type() const
{
    return "supervisedclassification";
}

SupervisedClassification::SupervisedClassification(const QString &name, const QString &description) : AnalysisPattern(name, description)
{
    Init();
}

SupervisedClassification::AnalysisPattern *SupervisedClassification::create(const QString &name, const QString &description, const IOOptions &options)
{
    SupervisedClassification *sc =  new SupervisedClassification(name, description);
    return sc;
}


