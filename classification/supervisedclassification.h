#ifndef SupervisedClassification_H
#define SupervisedClassification_H

#include <QObject>

#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "supervisedclassificationmodel.h"

namespace Ilwis{
namespace SClassification {

class SupervisedClassification : public AnalysisPattern
{

public:

    explicit SupervisedClassification();
    SupervisedClassification(const QString& name, const QString& description);
    bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters);
    Ilwis::IWorkflow workflow(const Ilwis::IOOptions& opt);
    virtual void store(QDataStream& stream);
    virtual void loadMetadata(QDataStream &stream);
    QString type() const;
   static AnalysisPattern *create(const QString& name, const QString& description, const Ilwis::IOOptions &options);

signals:

public slots:

private:
    void Init();

    NEW_ANALYSISPATTERN
};
}
}

#endif // SupervisedClassification_H
