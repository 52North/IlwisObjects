#ifndef SupervisedClassification_H
#define SupervisedClassification_H

#include <QObject>

#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "operationmetadata.h"
#include "raster.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "supervisedclassificationmodel.h"

namespace Ilwis{
namespace SClassification {

	const QString SCRASTERKEY = "multispectralraster";
	const QString SCDOMAINKEY = "classifierdomain";

class SupervisedClassification : public AnalysisPattern
{
	Q_OBJECT
public:

    explicit SupervisedClassification();
    SupervisedClassification(const QString& name, const QString& description);
    bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters);
    Ilwis::IWorkflow workflow(const Ilwis::IOOptions& opt);
    virtual void store(QDataStream& stream);
    virtual void loadMetadata(QDataStream &stream);
    QString type() const;
	virtual void addData(const QString& key, const QVariant& var);
	virtual QVariant data(const QString& key) const;
   static AnalysisPattern *create(const QString& name, const QString& description, const Ilwis::IOOptions &options);

signals:

public slots:

private:
    void Init();

	IRasterCoverage _multiSpectralRaster;
	IDomain _items;
	ITable _statistics;

    NEW_ANALYSISPATTERN
};
}
}

#endif // SupervisedClassification_H
