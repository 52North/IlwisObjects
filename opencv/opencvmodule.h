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

#ifndef OpenCVModule_H
#define OpenCVModule_H

#include <QObject>
#include <QLibrary>
#include "kernel.h"

namespace Ilwis {
namespace OpenCV {
class OpenCVModule : public Module
{
public:
    Q_OBJECT
    Q_INTERFACES(Ilwis::Module)
public:
    explicit OpenCVModule(QObject *parent = 0);
    virtual ~OpenCVModule();

    QString getInterfaceVersion() const;
    QString getName() const;
    QString getVersion() const;
    void getOperations(QVector<ICommandInfo *>& ) const{}
    void prepare();

    Q_PLUGIN_METADATA(IID "n52.ilwis.opencvoperations" FILE  "opencvoperations.json")
private:


signals:
};
}
}

#endif // OpenCVModule_H
