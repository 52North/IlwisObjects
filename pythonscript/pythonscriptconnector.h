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

#ifndef PYTHONSCRIPTCONNECTOR_H
#define PYTHONSCRIPTCONNECTOR_H

namespace Ilwis{
namespace PythonScript {
class PythonScriptConnector : public Ilwis::IlwisObjectConnector
{
public:
    PythonScriptConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool store(IlwisObject *obj, const IOOptions& options = IOOptions() );
    QString type() const;
    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());
    QString provider() const;
    bool loadMetaData(IlwisObject *obj, const Ilwis::IOOptions &options);
    bool loadData(IlwisObject *, const IOOptions &options = IOOptions());
};
}
}

#endif // PYTHONSCRIPTCONNECTOR_H
