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

#ifndef MODEL_H
#define MODEL_H

#include "kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT Model : public IlwisObject
{
public:
    Model();
    Model(const Resource &resource);

    IlwisTypes ilwisType() const;

    qint32 workflowCount() const;
    IWorkflow workflow(qint32 index) const;
    IWorkflow workflow(const QString& name) const;
    bool addWorklfow(const IWorkflow& wf);
    void removeWorkflow(const QString& name);
    void removeWorkflow(qint32 index);


    qint32 analysisCount() const;
	UPAnalysisPattern& analysisPattern(qint32 index) ;
	UPAnalysisPattern& analysisPattern(const QString& name) ;
    bool addAnalysisPattern(AnalysisPattern *pattern);
    void removeAnalysisPattern(const QString& name);
    void removeAnalysisPattern(qint32 index);

    qint32 applicationCount() const;
    SPModelApplication application(qint32 index) const;
    SPModelApplication application(const QString& name) const;
    bool addApplication(ApplicationModel *app);
    void removeApplication(const QString& name);
    void removeApplication(qint32 index);

private:
    std::vector<IWorkflow> _workflows;
    std::vector<UPAnalysisPattern> _analysisPatterns;
    std::vector<SPModelApplication> _applications;
};

typedef Ilwis::IlwisData<Ilwis::Model> IModel;
}



#endif // MODEL_H
