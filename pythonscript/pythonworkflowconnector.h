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

#ifndef PYTHONWORKFLOWCONNECTOR_H
#define PYTHONWORKFLOWCONNECTOR_H

#include <QBuffer>

namespace Ilwis{
class Workflow;

namespace PythonScript {
class PythonWorkflowConnector : public Ilwis::IlwisObjectConnector
{
public:
    PythonWorkflowConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool store(IlwisObject *, const IOOptions& options = IOOptions() );
    QString type() const;
    Ilwis::IlwisObject *create() const;
    QString provider() const;
    QVariant getProperty(const QString &name) const;

    static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());
    static ConnectorInterface *create2(const Ilwis::Resource &resource, bool load=true, const IOOptions &options=IOOptions());

private:
    std::unique_ptr<QIODevice> _datasource;
    OperationExpression _expression;
    QStringList _outputNames;
    QByteArray _data;

    bool openTarget();
};
}
}

#endif // PYTHONWORKFLOWCONNECTOR_H
