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

#ifndef ANALYSISMODEL_H
#define ANALYSISMODEL_H

#include <QObject>
#include "ilwiscoreui_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "location.h"
#include "operationmetadata.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"

namespace Ilwis{
    class AnalysisPattern;

namespace Ui{

#define NEW_ANALYSISMODEL \
    private: \
static AnalysisModel *dummy_analysis;

#define REGISTER_ANALYSISMODEL(classname,modelname) \
    AnalysisModel *classname::dummy_analysis = modelbuilder()->registerAnalysisModel(modelname, classname::create);


class ILWISCOREUISHARED_EXPORT AnalysisModel : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QString longName READ longName WRITE longName NOTIFY longNameChanged)

    explicit AnalysisModel(Ilwis::AnalysisPattern *pattern = 0, QObject *parent=0);
    Q_INVOKABLE void suicide();
    Q_INVOKABLE QString panel(const QString& panelName);

    QString name() const;

signals:
    void longNameChanged();
public slots:

protected:
    std::map<QString, QString> _panels;
    Ilwis::AnalysisPattern *_analysis = 0; // pattern is stored in the model; there is no ownership here

private:


    QString longName() const;
    void longName(const QString& lname);
    Ilwis::Resource& resourceRef();
    const Ilwis::Resource &resourceRef() const;
};
}
}

#endif // ANALYSISMODEL_H
