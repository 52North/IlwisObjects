/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef ANALYSISPATTERN_H
#define ANALYSISPATTERN_H
#include "kernel_global.h"

#define NEW_ANALYSISPATTERN \
    private: \
static Ilwis::AnalysisPattern *dummy_analysis;

#define REGISTER_ANALYSISPATTERN(classname) \
    AnalysisPattern *classname::dummy_analysis = ModellerFactory::registerAnalysisPattern(#classname, classname::create);

namespace Ilwis {
class Workfolow;
typedef IlwisData<Workflow> IWorkflow;
class Model;
typedef IlwisData<Model> IModel;

class KERNELSHARED_EXPORT AnalysisPattern : public QObject, public Identity
{
public:
    AnalysisPattern();
    AnalysisPattern(const QString& name, const QString& description=sUNDEF);

    virtual bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters)  = 0;
    virtual IWorkflow workflow(const IOOptions& opt) = 0;

    virtual void store(QDataStream& stream);
    virtual void loadMetadata(QDataStream& stream);
    virtual void loadData(QDataStream& stream);
	virtual void addData(const QString& key, const QVariant& var);
	virtual QVariant data(const QString& key) const;
    virtual QString type() const = 0 ;

    void attachedModel(quint64 modelid);
    IModel attachedModel();

private:
    quint64 _modelId; // the pattern maybe attached before the IModel realy exists (during prepare when a model is created)
    IModel _attachedModel;
};

typedef std::unique_ptr<AnalysisPattern> UPAnalysisPattern;
}

#endif // ANALYSISPATTERN_H
