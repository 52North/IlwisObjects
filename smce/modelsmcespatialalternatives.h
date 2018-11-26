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

#ifndef MODELSMCESPATIALALTERNATIVES_H
#define MODELSMCESPATIALALTERNATIVES_H

#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/workflow.h"
#include "modeller/applicationmodel.h"

namespace Ilwis{
class ApplicationModel;

namespace Smce {
class SMCESpatialAlternativesModel : public ApplicationModel
{
public:
    SMCESpatialAlternativesModel();
    void store(QDataStream& stream);
    void loadMetadata(QDataStream& stream);
    virtual void loadData(QDataStream& stream);
    QString type() const;
    static ApplicationModel *create();

NEW_MODELAPPLICATION
};
}
}

#endif // MODELSMCESPATIALALTERNATIVES_H
